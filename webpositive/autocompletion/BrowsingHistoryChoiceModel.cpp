/*
 * Copyright 2010 Stephan Aßmus <superstippi@gmx.de>
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "BeOSCompatibility.h"
#include "BrowsingHistoryChoiceModel.h"

#include <limits.h>

#include "BaseURL.h"
#include "BrowsingHistory.h"
#include "URLChoice.h"


BrowsingHistoryChoiceModel::BrowsingHistoryChoiceModel(BrowsingHistory* history)
	:
	fHistory(history),
	fChoices(20)
{
}


BrowsingHistoryChoiceModel::~BrowsingHistoryChoiceModel()
{
}


void
BrowsingHistoryChoiceModel::FetchChoicesFor(const BString& pattern)
{
	fChoices.MakeEmpty();

	// Search through BrowsingHistory for any matches.
	BrowsingHistory* history = fHistory != MY_NULLPTR
		? fHistory : BrowsingHistory::DefaultInstance();

	if (history == MY_NULLPTR || !history->Lock())
		return;

	BString lastBaseURL;
	int32 priority = INT_MAX;

	int32 count = history->CountItems();
	for (int32 i = 0; i < count; i++) {
		const BrowsingHistoryItem* item = history->ItemAt(i);
		if (item == MY_NULLPTR)
			continue;
		const BString& choiceText = item->URL();
		int32 matchPos = choiceText.IFindFirst(pattern);
		if (matchPos < 0)
			continue;
		if (lastBaseURL.Length() > 0
			&& choiceText.FindFirst(lastBaseURL) >= 0) {
			priority--;
		} else
			priority = INT_MAX;
		lastBaseURL = baseURL(choiceText);
		fChoices.AddItem(new URLChoice(choiceText,
			choiceText, matchPos, pattern.Length(), priority));
	}

	history->Unlock();

	fChoices.SortItems(_CompareChoices);
}


int32
BrowsingHistoryChoiceModel::CountChoices() const
{
	return fChoices.CountItems();
}


const BAutoCompleter::Choice*
BrowsingHistoryChoiceModel::ChoiceAt(int32 index) const
{
	return fChoices.ItemAt(index);
}


/*static*/ int
BrowsingHistoryChoiceModel::_CompareChoices(const BAutoCompleter::Choice* a,
	const BAutoCompleter::Choice* b)
{
	const URLChoice* aChoice = static_cast<const URLChoice*>(a);
	const URLChoice* bChoice = static_cast<const URLChoice*>(b);

	if (*aChoice < *bChoice)
		return -1;
	else if (*aChoice == *bChoice)
		return 0;
	return 1;
}
