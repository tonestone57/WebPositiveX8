/*
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef BROWSING_HISTORY_H
#define BROWSING_HISTORY_H

#include <memory>
#include <unordered_map>
#include <vector>

#include "DateTime.h"
#include <Locker.h>
#include <ObjectList.h>
#include <OS.h>

#include <String.h>

class BFile;


class BrowsingHistoryItem;
typedef std::shared_ptr<const BrowsingHistoryItem> BrowsingHistoryItemPtr;


class BrowsingHistoryItem {
public:
								BrowsingHistoryItem(const BString& url);
								BrowsingHistoryItem(
									const BrowsingHistoryItem& other);
								BrowsingHistoryItem(const BMessage* archive);
								~BrowsingHistoryItem();

			status_t			Archive(BMessage* archive) const;

			BrowsingHistoryItem& operator=(const BrowsingHistoryItem& other);

			bool				operator==(
									const BrowsingHistoryItem& other) const;
			bool				operator!=(
									const BrowsingHistoryItem& other) const;
			bool				operator<(
									const BrowsingHistoryItem& other) const;
			bool				operator<=(
									const BrowsingHistoryItem& other) const;
			bool				operator>(
									const BrowsingHistoryItem& other) const;
			bool				operator>=(
									const BrowsingHistoryItem& other) const;

			const BString&		URL() const { return fURL; }
			const BDateTime&	DateTime() const { return fDateTime; }
			uint32				InvokationCount() const {
									return fInvokationCount; }
			void				Invoked();

private:
			BString				fURL;
			BDateTime			fDateTime;
			uint32				fInvokationCount;
};


struct BStringHash {
	size_t operator()(const BString& s) const;
};


class BrowsingHistory : public BLocker {
public:
	static	BrowsingHistory*	DefaultInstance();

	virtual	bool				AddItem(const BrowsingHistoryItem& item);
	virtual	bool				RemoveItem(const BString& url);

	// Should Lock() the object when using these in some loop or so:
	virtual	int32				CountItems() const;
			BrowsingHistoryItem	HistoryItemAt(int32 index) const;

	// Requires the object to be Lock()ed:
	virtual	const BrowsingHistoryItem* ItemAt(int32 index) const;

	virtual	void				Clear();

			void				SetMaxHistoryItemAge(int32 days);
			int32				MaxHistoryItemAge() const;

protected:
								BrowsingHistory();
								BrowsingHistory(bool startThreads);

	virtual						~BrowsingHistory();

private:
			void				_Clear();
			bool				_AddItem(const BrowsingHistoryItem& item,
									bool invoke);
			int32				_InsertionIndex(
									const BrowsingHistoryItem* item) const;

			void				_LoadSettings();
			void				_SaveSettings(bool force = false);

	static	status_t			_SaveThread(void* data);
	static	status_t			_LoadThread(void* data);

private:
			std::vector<BrowsingHistoryItemPtr> fHistoryItems;
			std::unordered_map<BString, BrowsingHistoryItemPtr, BStringHash>
								fHistoryMap;
			int32				fMaxHistoryItemAge;

	static	BrowsingHistory		sDefaultInstance;
			bool				fSettingsLoaded;

			thread_id			fSaveThread;
			thread_id			fLoadThread;
			sem_id				fSaveSem;
			bool				fQuitting;
			std::unique_ptr<std::vector<BrowsingHistoryItemPtr>>
								fPendingSaveItems;
			BLocker				fSaveLock;
			BLocker				fFileLock;

			bigtime_t			fLastSaveTime;
};


#endif // BROWSING_HISTORY_H

