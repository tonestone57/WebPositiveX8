/*
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef BROWSING_HISTORY_H
#define BROWSING_HISTORY_H

#include "BeOSCompatibility.h"
#include "DateTime.h"
#include <Locker.h>
#include <ObjectList.h>
#include <OS.h>
#include <String.h>

class BFile;


class BrowsingHistoryItem : public BReferenceable {
public:
								BrowsingHistoryItem(const BString& url);
								BrowsingHistoryItem(const BString& url,
									const BDateTime& dateTime);
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
			uint32				InvocationCount() const {
									return fInvocationCount; }
			void				Invoked();

private:
			BString				fURL;
			BDateTime			fDateTime;
			uint32				fInvocationCount;
};


class BrowsingHistory : public BLocker {
	friend class BrowsingHistoryTest;
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
			typedef BObjectList<BrowsingHistoryItem> HistoryList;

			HistoryList			fHistoryItems;
			int32				fMaxHistoryItemAge;

	static	BrowsingHistory		sDefaultInstance;
			bool				fSettingsLoaded;

			thread_id			fSaveThread;
			thread_id			fLoadThread;
			sem_id				fSaveSem;
			bool				fQuitting;
			HistoryList*		fPendingSaveItems;
			BLocker				fSaveLock;
			BLocker				fFileLock;

			bigtime_t			fLastSaveTime;
};


#endif // BROWSING_HISTORY_H

