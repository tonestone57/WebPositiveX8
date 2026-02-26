#ifndef _MOCK_BSTRING_H
#define _MOCK_BSTRING_H
#include <string>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <stdio.h>
#include "SupportDefs.h"
class BString {
public:
    BString() {}
    BString(const char* s) : fString(s ? s : "") {}
    BString(const char* s, int32 length) : fString(s ? s : "", length) {}
    BString(const BString& other) : fString(other.fString) {}
    const char* String() const { return fString.c_str(); }
    int32 Length() const { return (int32)fString.length(); }
    bool operator==(const BString& other) const { return fString == other.fString; }
    bool operator!=(const BString& other) const { return fString != other.fString; }
    bool operator<(const BString& other) const { return fString < other.fString; }
    bool operator>(const BString& other) const { return fString > other.fString; }
    BString& operator=(const char* s) { fString = s ? s : ""; return *this; }
    BString& operator=(const BString& other) { fString = other.fString; return *this; }
    BString& operator<<(const char* s) { fString += (s ? s : ""); return *this; }
    BString& operator<<(int32 i) { fString += std::to_string(i); return *this; }
    BString& operator<<(const BString& s) { fString += s.fString; return *this; }

    int ICompare(const BString& other) const {
        return strcasecmp(fString.c_str(), other.fString.c_str());
    }
    BString& Remove(int32 start, int32 length) {
        if (start < (int32)fString.length()) {
            if (start + length > (int32)fString.length())
                length = (int32)fString.length() - start;
            fString.erase((size_t)start, (size_t)length);
        }
        return *this;
    }
    BString& Insert(const char* s, int32 pos) {
        if (pos <= (int32)fString.length())
            fString.insert((size_t)pos, s ? s : "");
        return *this;
    }
    BString& Insert(const BString& s, int32 pos) {
        return Insert(s.String(), pos);
    }
    void MoveInto(BString& into, int32 from, int32 length) {
        if (from < (int32)fString.length()) {
            if (from + length > (int32)fString.length())
                length = (int32)fString.length() - from;
            into.fString = fString.substr((size_t)from, (size_t)length);
            fString.erase((size_t)from, (size_t)length);
        }
    }
    BString& SetTo(const char* s) { fString = s ? s : ""; return *this; }
    BString& SetTo(const char* s, int32 length) { fString.assign(s ? s : "", length); return *this; }

    int32 FindFirst(const char* s, int32 fromIndex = 0) const {
        size_t pos = fString.find(s ? s : "", (size_t)fromIndex);
        return (pos == std::string::npos) ? -1 : (int32)pos;
    }
    int32 FindFirst(const BString& s, int32 fromIndex = 0) const {
        return FindFirst(s.String(), fromIndex);
    }
    int32 IFindFirst(const BString& s, int32 fromIndex = 0) const {
        // Simple case-insensitive find
        if (fromIndex >= (int32)fString.length()) return -1;
        const char* haystack = fString.c_str();
        const char* needle = s.String();
        for (int32 i = fromIndex; i <= (int32)fString.length() - s.Length(); i++) {
            if (strncasecmp(haystack + i, needle, s.Length()) == 0) return i;
        }
        return -1;
    }

    void SetToFormat(const char* format, ...) {
        char buffer[1024];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        fString = buffer;
    }

private:
    std::string fString;
};
#endif
