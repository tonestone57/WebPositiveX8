#ifndef HASH_STRING_H
#define HASH_STRING_H
#include <String.h>
namespace BPrivate {
class HashString : public BString {
public:
    HashString() : BString() {}
    HashString(const char* s) : BString(s) {}
    HashString(const BString& s) : BString(s) {}
};
}
using BPrivate::HashString;
#endif
