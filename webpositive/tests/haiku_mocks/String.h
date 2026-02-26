#ifndef _MOCK_STRING_H
#define _MOCK_STRING_H
#include <string>
#include <cstring>
#include <stdarg.h>
#include <cstdio>
#include "SupportDefs.h"

class BString {
public:
    BString() {}
    BString(const char* s) : fString(s ? s : "") {}
    BString(const char* s, int32 maxLength) : fString(s ? s : "", maxLength) {}
    BString(const BString& other) : fString(other.fString) {}

    const char* String() const { return fString.c_str(); }
    operator const char*() const { return fString.c_str(); }
    int32 Length() const { return (int32)fString.length(); }
    bool IsEmpty() const { return fString.empty(); }

    BString& operator=(const char* s) { fString = s ? s : ""; return *this; }
    BString& operator=(const BString& other) { fString = other.fString; return *this; }
    BString& operator<<(const char* s) { fString += (s ? s : ""); return *this; }
    BString& operator<<(int32 i) { fString += std::to_string(i); return *this; }
    BString& operator<<(uint32 i) { fString += std::to_string(i); return *this; }
    BString& operator<<(const BString& s) { fString += s.fString; return *this; }
    BString& operator+=(const char* s) { fString += (s ? s : ""); return *this; }
    BString& operator+=(const BString& s) { fString += s.fString; return *this; }

    bool operator==(const char* s) const { return fString == (s ? s : ""); }
    bool operator==(const BString& other) const { return fString == other.fString; }
    bool operator!=(const char* s) const { return fString != (s ? s : ""); }
    bool operator!=(const BString& other) const { return fString != other.fString; }
    bool operator<(const BString& other) const { return fString < other.fString; }

    int ICompare(const BString& other) const {
        return strcasecmp(fString.c_str(), other.fString.c_str());
    }
    int Compare(const char* s, int32 n) const {
        if (!s) return 1;
        return strncmp(fString.c_str(), s, n);
    }

    BString& Remove(int32 start, int32 length) {
        if (start < (int32)fString.length()) {
            if (start + length > (int32)fString.length())
                length = (int32)fString.length() - start;
            fString.erase((size_t)start, (size_t)length);
        }
        return *this;
    }
    BString& Truncate(int32 length) {
        if (length < (int32)fString.length())
            fString.erase((size_t)length);
        return *this;
    }

    BString& Insert(const char* s, int32 pos) {
        if (pos <= (int32)fString.length())
            fString.insert((size_t)pos, s ? s : "");
        return *this;
    }

    int32 FindFirst(const char* s, int32 fromIndex = 0) const {
        if (!s) return -1;
        size_t pos = fString.find(s, (size_t)fromIndex);
        return (pos == std::string::npos) ? -1 : (int32)pos;
    }
    int32 FindFirst(char c, int32 fromIndex = 0) const {
        size_t pos = fString.find(c, (size_t)fromIndex);
        return (pos == std::string::npos) ? -1 : (int32)pos;
    }
    int32 FindLast(char c) const {
        size_t pos = fString.find_last_of(c);
        return (pos == std::string::npos) ? -1 : (int32)pos;
    }

    int32 IFindFirst(const char* s, int32 fromIndex = 0) const {
        if (!s || fromIndex >= (int32)fString.length()) return -1;
        const char* haystack = fString.c_str();
        size_t len = strlen(s);
        for (int32 i = fromIndex; i <= (int32)fString.length() - (int32)len; i++) {
            if (strncasecmp(haystack + i, s, len) == 0) return i;
        }
        return -1;
    }

    void SetToFormat(const char* format, ...) {
        if (!format) return;
        char buffer[1024];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        fString = buffer;
    }

    BString& ReplaceAll(const char* oldS, const char* newS) {
        if (!oldS || !newS) return *this;
        size_t pos = 0;
        std::string s_old(oldS);
        std::string s_new(newS);
        while((pos = fString.find(s_old, pos)) != std::string::npos) {
            fString.replace(pos, s_old.length(), s_new);
            pos += s_new.length();
        }
        return *this;
    }

    BString& ReplaceAll(char oldC, char newC) {
        for (auto& c : fString) if (c == oldC) c = newC;
        return *this;
    }

    BString& ReplaceFirst(const char* oldS, const char* newS) {
        if (!oldS || !newS) return *this;
        size_t pos = fString.find(oldS);
        if (pos != std::string::npos) {
            fString.replace(pos, strlen(oldS), newS);
        }
        return *this;
    }

    BString& MoveInto(BString& into, int32 from, int32 length) {
        if (from < 0 || from >= (int32)fString.length()) return *this;
        into.fString = fString.substr(from, length);
        fString.erase(from, length);
        return *this;
    }

    BString& SetTo(const char* s) { fString = s ? s : ""; return *this; }
    BString& SetTo(const char* s, int32 length) { fString.assign(s ? s : "", length); return *this; }

private:
    std::string fString;
};
#endif
