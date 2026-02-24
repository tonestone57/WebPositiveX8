/*
 * Copyright 2010 Stephan Aßmus <superstippi@gmx.de>
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef URL_CHOICE_H
#define URL_CHOICE_H

#include <String.h>
#include <SupportDefs.h>

#include "AutoCompleter.h"

class URLChoice : public BAutoCompleter::Choice {
public:
	URLChoice(const BString& choiceText, const BString& displayText,
			int32 matchPos, int32 matchLen, int32 priority)
		:
		BAutoCompleter::Choice(choiceText, displayText, matchPos, matchLen),
		fPriority(priority)
	{
	}

	bool operator<(const URLChoice& other) const
	{
		if (fPriority != other.fPriority)
			return fPriority > other.fPriority;
		return DisplayText() < other.DisplayText();
	}

	bool operator==(const URLChoice& other) const
	{
		return fPriority == other.fPriority
			&& DisplayText() == other.DisplayText();
	}

private:
	int32 fPriority;
};

#endif // URL_CHOICE_H
