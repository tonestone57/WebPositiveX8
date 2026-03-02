#ifndef HASH_STRING_H
#define HASH_STRING_H

#include <String.h>
#include <functional>

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
		const char* str = s.String();
		if (str == nullptr)
			return 0;
		// Simple FNV-1a hash for performance
		size_t h = 0x811c9dc5;
		while (*str) {
			h ^= (unsigned char)*str++;
			h *= 0x01000193;
		}
		return h;
	}
};

} // namespace std

#endif // HASH_STRING_H
