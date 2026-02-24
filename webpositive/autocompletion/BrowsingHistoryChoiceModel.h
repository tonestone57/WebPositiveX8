/*
 * Copyright 2010 Stephan Aßmus <superstippi@gmx.de>
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef BROWSING_HISTORY_CHOICE_MODEL_H
#define BROWSING_HISTORY_CHOICE_MODEL_H

#include <ObjectList.h>
#include "AutoCompleter.h"

class BrowsingHistory;

class BrowsingHistoryChoiceModel : public BAutoCompleter::ChoiceModel {
public:
								BrowsingHistoryChoiceModel(
									BrowsingHistory* history = NULL);
	virtual						~BrowsingHistoryChoiceModel();

	virtual void				FetchChoicesFor(const BString& pattern);

	virtual int32				CountChoices() const;
	virtual const BAutoCompleter::Choice* ChoiceAt(int32 index) const;

private:
	static	int					_CompareChoices(const BAutoCompleter::Choice* a,
									const BAutoCompleter::Choice* b);

			BrowsingHistory*	fHistory;
			BObjectList<BAutoCompleter::Choice, true> fChoices;
};

#endif // BROWSING_HISTORY_CHOICE_MODEL_H
