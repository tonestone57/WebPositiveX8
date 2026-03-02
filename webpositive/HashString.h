#ifndef HASH_STRING_H
#define HASH_STRING_H

#include <String.h>
#include <functional>
#include <string>

namespace BPrivate {

class HashString : public BString {
public:
	HashString() : BString() {}
	HashString(const char* s) : BString(s) {}
	HashString(const BString& s) : BString(s) {}
};

} // namespace BPrivate

using BPrivate::HashString;

namespace std {

template<>
struct hash<HashString> {
	size_t operator()(const HashString& s) const
	{
		return hash<string>()(s.String());
	}
};

} // namespace std

#endif // HASH_STRING_H
