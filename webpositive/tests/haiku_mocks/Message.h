#ifndef _MESSAGE_H
#define _MESSAGE_H
#include "SupportDefs.h"
#include "String.h"
#include <map>
#include <vector>

#define B_MIME_TYPE 'MIME'

class BFile;
class BMessage {
public:
    BMessage() : what(0) {}
    BMessage(uint32 what) : what(what) {}
    status_t FindMessage(const char* name, int32 index, BMessage* message) const { return B_ERROR; }
    status_t FindMessage(const char* name, BMessage* message) const { return B_ERROR; }
    status_t AddMessage(const char* name, const BMessage* message) { return B_OK; }
    status_t AddString(const char* name, const char* string) { fStrings[name] = string ? string : ""; return B_OK; }
    status_t AddString(const char* name, const BString& string) { return AddString(name, string.String()); }
    status_t AddUInt32(const char* name, uint32 value) { fUInts[name] = value; return B_OK; }
    status_t AddInt32(const char* name, int32 value) { fInts[name] = value; return B_OK; }
    status_t FindString(const char* name, BString* string) const {
        auto it = fStrings.find(name);
        if (it != fStrings.end()) { *string = it->second.c_str(); return B_OK; }
        return B_NAME_NOT_FOUND;
    }
    BString FindString(const char* name, int32 index = 0) const {
        BString s;
        FindString(name, index, &s);
        return s;
    }
    status_t FindString(const char* name, int32 index, BString* string) const {
        if (index == 0) return FindString(name, string);
        return B_NAME_NOT_FOUND;
    }
    status_t FindUInt32(const char* name, uint32* value) const {
        auto it = fUInts.find(name);
        if (it != fUInts.end()) { *value = it->second; return B_OK; }
        return B_NAME_NOT_FOUND;
    }
    uint32 FindUInt32(const char* name, int32 index = 0) const {
        uint32 val = 0;
        FindUInt32(name, &val);
        return val;
    }
    status_t FindInt32(const char* name, int32* value) const {
        auto it = fInts.find(name);
        if (it != fInts.end()) { *value = it->second; return B_OK; }
        return B_NAME_NOT_FOUND;
    }
    int32 FindInt32(const char* name, int32 index = 0) const {
        int32 val = 0;
        FindInt32(name, &val);
        return val;
    }
    status_t Flatten(BFile* file) const { return B_OK; }
    status_t Unflatten(BFile* file) { return B_OK; }
    void MakeEmpty() { fStrings.clear(); fUInts.clear(); fInts.clear(); }
    status_t AddData(const char* name, uint32 type, const void* data, ssize_t numBytes) { return B_OK; }

    uint32 what;
private:
    std::map<std::string, std::string> fStrings;
    std::map<std::string, uint32> fUInts;
    std::map<std::string, int32> fInts;
};
#endif
