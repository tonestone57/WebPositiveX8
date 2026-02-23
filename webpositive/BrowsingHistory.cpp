/*
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "BrowsingHistory.h"

#include <new>
#include <stdio.h>
#include <sys/stat.h>

#include <Autolock.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <Message.h>
#include <Path.h>

#include "BrowserApp.h"


BrowsingHistoryItem::BrowsingHistoryItem(const BString& url)
	:
	fURL(url),
	fDateTime(BDateTime::CurrentDateTime(B_LOCAL_TIME)),
	fInvokationCount(0)
{
}


BrowsingHistoryItem::BrowsingHistoryItem(const BrowsingHistoryItem& other)
{
	*this = other;
}


BrowsingHistoryItem::BrowsingHistoryItem(const BMessage* archive)
{
	if (!archive)
		return;
	BMessage dateTimeArchive;
	if (archive->FindMessage("date time", &dateTimeArchive) == B_OK)
		fDateTime = BDateTime(&dateTimeArchive);
	archive->FindString("url", &fURL);
	archive->FindUInt32("invokations", &fInvokationCount);
}


BrowsingHistoryItem::~BrowsingHistoryItem()
{
}


status_t
BrowsingHistoryItem::Archive(BMessage* archive) const
{
	if (!archive)
		return B_BAD_VALUE;
	BMessage dateTimeArchive;
	status_t status = fDateTime.Archive(&dateTimeArchive);
	if (status == B_OK)
		status = archive->AddMessage("date time", &dateTimeArchive);
	if (status == B_OK)
		status = archive->AddString("url", fURL.String());
	if (status == B_OK)
		status = archive->AddUInt32("invokations", fInvokationCount);
	return status;
}


BrowsingHistoryItem&
BrowsingHistoryItem::operator=(const BrowsingHistoryItem& other)
{
	if (this == &other)
		return *this;

	fURL = other.fURL;
	fDateTime = other.fDateTime;
	fInvokationCount = other.fInvokationCount;

	return *this;
}


bool
BrowsingHistoryItem::operator==(const BrowsingHistoryItem& other) const
{
	if (this == &other)
		return true;

	return fURL == other.fURL && fDateTime == other.fDateTime
		&& fInvokationCount == other.fInvokationCount;
}


bool
BrowsingHistoryItem::operator!=(const BrowsingHistoryItem& other) const
{
	return !(*this == other);
}


bool
BrowsingHistoryItem::operator<(const BrowsingHistoryItem& other) const
{
	if (this == &other)
		return false;

	return fDateTime < other.fDateTime || fURL < other.fURL;
}


bool
BrowsingHistoryItem::operator<=(const BrowsingHistoryItem& other) const
{
	return (*this == other) || (*this < other);
}


bool
BrowsingHistoryItem::operator>(const BrowsingHistoryItem& other) const
{
	if (this == &other)
		return false;

	return fDateTime > other.fDateTime || fURL > other.fURL;
}


bool
BrowsingHistoryItem::operator>=(const BrowsingHistoryItem& other) const
{
	return (*this == other) || (*this > other);
}


void
BrowsingHistoryItem::Invoked()
{
	// Eventually, we may overflow...
	uint32 count = fInvokationCount + 1;
	if (count > fInvokationCount)
		fInvokationCount = count;
	fDateTime = BDateTime::CurrentDateTime(B_LOCAL_TIME);
}


// #pragma mark - BrowsingHistory


BrowsingHistory
BrowsingHistory::sDefaultInstance;


BrowsingHistory::BrowsingHistory()
	:
	BLocker("browsing history"),
	fHistoryItems(64),
	fMaxHistoryItemAge(7),
	fSettingsLoaded(false),
	fQuitting(false),
	fPendingSaveItems(nullptr),
	fSaveLock("browsing history save lock"),
	fFileLock("browsing history file lock")
{
	fSaveSem = create_sem(0, "browsing history save sem");
	fSaveThread = spawn_thread(_SaveThread, "browsing history saver",
		B_LOW_PRIORITY, this);
	fLoadThread = -1;
	resume_thread(fSaveThread);
}


BrowsingHistory::~BrowsingHistory()
{
	if (fLoadThread >= 0) {
		status_t exitValue;
		wait_for_thread(fLoadThread, &exitValue);
	}

	// Queue a final save
	_SaveSettings();

	fQuitting = true;
	release_sem(fSaveSem);

	status_t exitValue;
	wait_for_thread(fSaveThread, &exitValue);

	delete_sem(fSaveSem);
	_Clear();
}


/*static*/ BrowsingHistory*
BrowsingHistory::DefaultInstance()
{
	if (sDefaultInstance.Lock()) {
		sDefaultInstance._LoadSettings();
		sDefaultInstance.Unlock();
	}
	return &sDefaultInstance;
}


bool
BrowsingHistory::AddItem(const BrowsingHistoryItem& item)
{
	BAutolock _(this);

	return _AddItem(item, false);
}


bool
BrowsingHistory::RemoveItem(const BString& url)
{
	BAutolock _(this);
	bool removed = false;
	for (int32 i = fHistoryItems.CountItems() - 1; i >= 0; i--) {
		BrowsingHistoryItem* item = fHistoryItems.ItemAt(i);
		if (item->URL() == url) {
			fHistoryMap.erase(item->URL().String());
			fHistoryItems.RemoveItem(i);
			delete item;
			removed = true;
		}
	}

	if (removed)
		_SaveSettings();

	return removed;
}


int32
BrowsingHistory::CountItems() const
{
	BAutolock _(const_cast<BrowsingHistory*>(this));

	return fHistoryItems.CountItems();
}


BrowsingHistoryItem
BrowsingHistory::HistoryItemAt(int32 index) const
{
	BAutolock _(const_cast<BrowsingHistory*>(this));

	const BrowsingHistoryItem* existingItem = ItemAt(index);
	if (!existingItem)
		return BrowsingHistoryItem(BString());

	return BrowsingHistoryItem(*existingItem);
}


const BrowsingHistoryItem*
BrowsingHistory::ItemAt(int32 index) const
{
	return fHistoryItems.ItemAt(index);
}


void
BrowsingHistory::Clear()
{
	BAutolock _(this);
	_Clear();
	_SaveSettings();
}	


void
BrowsingHistory::SetMaxHistoryItemAge(int32 days)
{
	BAutolock _(this);
	if (fMaxHistoryItemAge != days) {
		fMaxHistoryItemAge = days;
		_SaveSettings();
	}
}	


int32
BrowsingHistory::MaxHistoryItemAge() const
{
	return fMaxHistoryItemAge;
}	


// #pragma mark - private


void
BrowsingHistory::_Clear()
{
	fHistoryItems.MakeEmpty();
	fHistoryMap.clear();
}


bool
BrowsingHistory::_AddItem(const BrowsingHistoryItem& item, bool internal)
{
	auto it = fHistoryMap.find(item.URL().String());
	if (it != fHistoryMap.end()) {
		if (!internal) {
			it->second->Invoked();
			_SaveSettings();
		}
		return true;
	}

	int32 count = CountItems();
	int32 insertionIndex = count;
	for (int32 i = 0; i < count; i++) {
		BrowsingHistoryItem* existingItem = fHistoryItems.ItemAt(i);
		if (item < *existingItem)
			insertionIndex = i;
	}
	BrowsingHistoryItem* newItem = new(std::nothrow) BrowsingHistoryItem(item);
	if (!newItem || !fHistoryItems.AddItem(newItem, insertionIndex)) {
		delete newItem;
		return false;
	}

	fHistoryMap[newItem->URL().String()] = newItem;

	if (!internal) {
		newItem->Invoked();
		_SaveSettings();
	}

	return true;
}


void
BrowsingHistory::_LoadSettings()
{
	// Only called with lock held
	if (fSettingsLoaded || fLoadThread >= 0)
		return;

	fLoadThread = spawn_thread(_LoadThread, "browsing history loader",
		B_LOW_PRIORITY, this);
	if (fLoadThread >= 0)
		resume_thread(fLoadThread);
}


void
BrowsingHistory::_SaveSettings()
{
	BAutolock _(this);

	// Create deep copy of items to save
	// BObjectList(..., true) owns the items and will delete them on destruction
	std::unique_ptr<BObjectList<BrowsingHistoryItem, true>> newItems(
		new(std::nothrow) BObjectList<BrowsingHistoryItem, true>(
			fHistoryItems.CountItems()));
	if (!newItems)
		return;

	int32 count = fHistoryItems.CountItems();
	for (int32 i = 0; i < count; i++) {
		const BrowsingHistoryItem* item = ItemAt(i);
		if (item)
			newItems->AddItem(new(std::nothrow) BrowsingHistoryItem(*item));
	}

	fSaveLock.Lock();
	fPendingSaveItems = std::move(newItems);
	fSaveLock.Unlock();

	release_sem(fSaveSem);
}


status_t
BrowsingHistory::_SaveThread(void* data)
{
	BrowsingHistory* self = (BrowsingHistory*)data;

	while (true) {
		acquire_sem(self->fSaveSem);

		std::unique_ptr<BObjectList<BrowsingHistoryItem, true>> itemsToSave;

		self->fSaveLock.Lock();
		itemsToSave = std::move(self->fPendingSaveItems);
		self->fSaveLock.Unlock();

		if (self->fQuitting && !itemsToSave)
			break;

		if (itemsToSave) {
			self->fFileLock.Lock();
			BFile settingsFile;
			if (self->_OpenSettingsFile(settingsFile,
					B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY)) {
				BMessage settingsArchive;

				self->Lock();
				int32 maxAge = self->fMaxHistoryItemAge;
				self->Unlock();

				settingsArchive.AddInt32("max history item age", maxAge);

				BMessage historyItemArchive;
				int32 count = itemsToSave->CountItems();
				for (int32 i = 0; i < count; i++) {
					BrowsingHistoryItem* item = itemsToSave->ItemAt(i);
					if (item && item->Archive(&historyItemArchive) == B_OK) {
						settingsArchive.AddMessage("history item", &historyItemArchive);
					}
					historyItemArchive.MakeEmpty();
				}
				settingsArchive.Flatten(&settingsFile);
			}
			self->fFileLock.Unlock();
		}

		if (self->fQuitting)
			break;
	}
	return B_OK;
}


status_t
BrowsingHistory::_LoadThread(void* data)
{
	BrowsingHistory* self = (BrowsingHistory*)data;

	self->fFileLock.Lock();
	BFile settingsFile;
	// We read into a BMessage first, then lock the main object to add items.
	// This keeps the critical section (reading/parsing) out of the main lock.
	BMessage settingsArchive;
	bool fileOpened = self->_OpenSettingsFile(settingsFile, B_READ_ONLY);
	if (fileOpened)
		settingsArchive.Unflatten(&settingsFile);
	self->fFileLock.Unlock();

	if (!fileOpened) {
		BAutolock _(self);
		self->fSettingsLoaded = true;
		return B_OK;
	}

	BAutolock _(self);
	// Double check if we were cleared or something while loading
	if (self->fSettingsLoaded)
		return B_OK;

	int32 maxAge = 7;
	if (settingsArchive.FindInt32("max history item age", &maxAge) == B_OK) {
		self->fMaxHistoryItemAge = maxAge;
	}

	BDateTime oldestAllowedDateTime
		= BDateTime::CurrentDateTime(B_LOCAL_TIME);
	oldestAllowedDateTime.Date().AddDays(-self->fMaxHistoryItemAge);

	BMessage historyItemArchive;
	for (int32 i = 0; settingsArchive.FindMessage("history item", i,
			&historyItemArchive) == B_OK; i++) {
		BrowsingHistoryItem item(&historyItemArchive);
		if (oldestAllowedDateTime < item.DateTime())
			self->_AddItem(item, true);
		historyItemArchive.MakeEmpty();
	}

	self->fSettingsLoaded = true;
	return B_OK;
}


bool
BrowsingHistory::_OpenSettingsFile(BFile& file, uint32 mode)
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) != B_OK
		|| path.Append(kApplicationName) != B_OK
		|| path.Append("BrowsingHistory") != B_OK) {
		return false;
	}
	if (file.SetTo(path.Path(), mode) != B_OK)
		return false;
	if ((mode & B_CREATE_FILE) != 0)
		file.SetPermissions(S_IRUSR | S_IWUSR);
	return true;
}

