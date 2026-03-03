/*
 * Copyright 2010 Stephan Aßmus <superstippi@gmx.de>
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "BaseURL.h"


BString
baseURL(const BString& string)
{
	int32 baseURLStart = string.FindFirst("://");
	if (baseURLStart < 0)
		return BString("");

	baseURLStart += 3;
	int32 baseURLEnd = string.FindFirst("/", baseURLStart);
	int32 queryStart = string.FindFirst("?", baseURLStart);
	int32 fragmentStart = string.FindFirst("#", baseURLStart);

	if (baseURLEnd < 0 || (queryStart >= 0 && queryStart < baseURLEnd))
		baseURLEnd = queryStart;
	if (baseURLEnd < 0 || (fragmentStart >= 0 && fragmentStart < baseURLEnd))
		baseURLEnd = fragmentStart;

	BString result;
	if (baseURLEnd < 0)
		result.SetTo(string.String() + baseURLStart);
	else
		result.SetTo(string.String() + baseURLStart, baseURLEnd - baseURLStart);
	return result;
}
