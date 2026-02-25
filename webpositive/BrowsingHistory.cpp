/*
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "BrowsingHistory.h"

#include <algorithm>
#include <new>
#include <string_view>
#include <stdio.h>
#include <sys/stat.h>

#include <Autolock.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <Message.h>
#include <Path.h>

#include "BrowserApp.h"
#include "SettingsFile.h"
#include "SettingsKeys.h"


BrowsingHistoryItem::BrowsingHistoryItem(const BString& url)
	:
	fURL(url),
	fDateTime(BDateTime::CurrentDateTime(B_LOCAL_TIME)),
	fInvokationCount(0)
{
}


BrowsingHistoryItem::BrowsingHistoryItem(const BString& url,
	const BDateTime& dateTime)
	:
	fURL(url),
	fDateTime(dateTime),
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

	if (fDateTime != other.fDateTime)
		return fDateTime < other.fDateTime;
	return fURL < other.fURL;
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

	if (fDateTime != other.fDateTime)
		return fDateTime > other.fDateTime;
	return fURL > other.fURL;
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
	BrowsingHistory(true)
{
}


BrowsingHistory::BrowsingHistory(bool startThreads)
	:
	BLocker("browsing history"),
	fMaxHistoryItemAge(7),
	fSettingsLoaded(false),
	fQuitting(false),
	fPendingSaveItems(nullptr),
	fSaveLock("browsing history save lock"),
	fFileLock("browsing history file lock"),
	fLastSaveTime(0)
{
	fHistoryItems = std::make_shared<HistoryVector>();
	fHistoryItems->reserve(64);
	if (startThreads) {
		fSaveSem = create_sem(0, "browsing history save sem");
		fSaveThread = spawn_thread(_SaveThread, "browsing history saver",
			B_LOW_PRIORITY, this);
		fLoadThread = -1;
		resume_thread(fSaveThread);
	} else {
		fSaveSem = -1;
		fSaveThread = -1;
		fLoadThread = -1;
	}
}


BrowsingHistory::~BrowsingHistory()
{
	if (fLoadThread >= 0) {
		status_t exitValue;
		wait_for_thread(fLoadThread, &exitValue);
	}

	// Queue a final save
	_SaveSettings(true);

	fQuitting = true;
	if (fSaveSem >= 0)
		release_sem(fSaveSem);

	if (fSaveThread >= 0) {
		status_t exitValue;
		wait_for_thread(fSaveThread, &exitValue);
	}

	if (fSaveSem >= 0)
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
	auto it = fHistoryMap.find(url);
	if (it == fHistoryMap.end())
		return false;

	BrowsingHistoryItemPtr item = it->second;

	_EnsureUniqueVector();
	auto it_list = std::find(fHistoryItems->begin(), fHistoryItems->end(), item);
	if (it_list != fHistoryItems->end())
		fHistoryItems->erase(it_list);

	fHistoryMap.erase(it);

	_SaveSettings();

	return true;
}


int32
BrowsingHistory::CountItems() const
{
	BAutolock _(const_cast<BrowsingHistory*>(this));

	return (int32)fHistoryItems->size();
}


BrowsingHistoryItem
BrowsingHistory::HistoryItemAt(int32 index) const
{
	BAutolock _(const_cast<BrowsingHistory*>(this));

	if (index < 0 || index >= (int32)fHistoryItems->size())
		return BrowsingHistoryItem(BString());

	return *(*fHistoryItems)[index];
}


const BrowsingHistoryItem*
BrowsingHistory::ItemAt(int32 index) const
{
	if (index < 0 || index >= (int32)fHistoryItems->size())
		return nullptr;
	return (*fHistoryItems)[index].get();
}


void
BrowsingHistory::Clear()
{
	BAutolock _(this);
	_Clear();
	_SaveSettings(true);
}	


void
BrowsingHistory::SetMaxHistoryItemAge(int32 days)
{
	BAutolock _(this);
	if (fMaxHistoryItemAge != days) {
		fMaxHistoryItemAge = days;
		_SaveSettings(true);
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
	fHistoryItems = std::make_shared<HistoryVector>();
	fHistoryMap.clear();
}


void
BrowsingHistory::_EnsureUniqueVector()
{
	if (fHistoryItems.use_count() > 1) {
		fHistoryItems = std::make_shared<HistoryVector>(*fHistoryItems);
	}
}


bool
BrowsingHistory::_AddItem(const BrowsingHistoryItem& item, bool internal)
{
	auto it = fHistoryMap.find(item.URL());
	if (it != fHistoryMap.end()) {
		if (!internal) {
			BrowsingHistoryItemPtr existingItem = it->second;

			_EnsureUniqueVector();
			auto it_list = std::find(fHistoryItems->begin(), fHistoryItems->end(), existingItem);
			if (it_list != fHistoryItems->end())
				fHistoryItems->erase(it_list);

			std::shared_ptr<BrowsingHistoryItem> newItem(new(std::nothrow) BrowsingHistoryItem(*existingItem));
			if (!newItem)
				return false;
			newItem->Invoked();

			BrowsingHistoryItemPtr itemToStore = newItem;
			int32 insertionIndex = _InsertionIndex(itemToStore.get());
			fHistoryItems->insert(fHistoryItems->begin() + insertionIndex, itemToStore);
			fHistoryMap[itemToStore->URL()] = itemToStore;

			_SaveSettings();
		}
		return true;
	}

	std::shared_ptr<BrowsingHistoryItem> newItem(new(std::nothrow) BrowsingHistoryItem(item));
	if (!newItem)
		return false;

	if (!internal)
		newItem->Invoked();

	BrowsingHistoryItemPtr itemToStore = newItem;
	int32 insertionIndex = _InsertionIndex(itemToStore.get());

	_EnsureUniqueVector();
	fHistoryItems->insert(fHistoryItems->begin() + insertionIndex, itemToStore);

	fHistoryMap[itemToStore->URL()] = itemToStore;

	if (!internal)
		_SaveSettings();

	return true;
}


int32
BrowsingHistory::_InsertionIndex(const BrowsingHistoryItem* item) const
{
	int32 count = (int32)fHistoryItems->size();
	if (count == 0 || *(*fHistoryItems)[count - 1] < *item)
		return count;

	auto it = std::lower_bound(fHistoryItems->begin(), fHistoryItems->end(),
		item,
		[](const BrowsingHistoryItemPtr& a, const BrowsingHistoryItem* b) {
			return *a < *b;
		});
	return (int32)std::distance(fHistoryItems->begin(), it);
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
BrowsingHistory::_SaveSettings(bool force)
{
	// Force save if requested, or if enough time has passed since last save.
	BAutolock _(this);

	if (!force && fLastSaveTime != 0
		&& system_time() - fLastSaveTime < 30000000) {
		return;
	}

	fLastSaveTime = system_time();

	// Take a reference to the current history vector. This is O(1) and safe
	// because of the COW logic on the vector itself.
	fSaveLock.Lock();
	fPendingSaveItems = fHistoryItems;
	fSaveLock.Unlock();

	if (fSaveSem >= 0)
		release_sem(fSaveSem);
}


status_t
BrowsingHistory::_SaveThread(void* data)
{
	BrowsingHistory* self = (BrowsingHistory*)data;

	while (true) {
		acquire_sem(self->fSaveSem);

		HistoryVectorPtr itemsToSave;

		self->fSaveLock.Lock();
		itemsToSave = std::move(self->fPendingSaveItems);
		self->fSaveLock.Unlock();

		if (self->fQuitting && !itemsToSave)
			break;

		if (itemsToSave) {
			self->fFileLock.Lock();
			BFile settingsFile;
			if (OpenSettingsFile(settingsFile, kSettingsFileNameBrowsingHistory,
					B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY) == B_OK) {
				BMessage settingsArchive;

				self->Lock();
				int32 maxAge = self->fMaxHistoryItemAge;
				self->Unlock();

				settingsArchive.AddInt32("max history item age", maxAge);

				BMessage historyItemArchive;
				int32 count = (int32)itemsToSave->size();
				for (int32 i = 0; i < count; i++) {
					const BrowsingHistoryItemPtr& item = (*itemsToSave)[i];
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
	bool fileOpened = OpenSettingsFile(settingsFile,
		kSettingsFileNameBrowsingHistory, B_READ_ONLY) == B_OK;
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



