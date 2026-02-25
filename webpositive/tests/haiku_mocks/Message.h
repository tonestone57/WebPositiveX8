#ifndef _MESSAGE_H
#define _MESSAGE_H
#include "SupportDefs.h"
#include "String.h"
#include <map>
#include <vector>

class BFile;
class BMessage {
public:
    BMessage() : what(0) {}
    BMessage(uint32 what) : what(what) {}
    status_t FindMessage(const char* name, int32 index, BMessage* message) const { return B_ERROR; }
    status_t FindMessage(const char* name, BMessage* message) const { return B_ERROR; }
    status_t AddMessage(const char* name, const BMessage* message) { return B_OK; }
    status_t AddString(const char* name, const char* string) { fStrings[name] = string; return B_OK; }
    status_t AddUInt32(const char* name, uint32 value) { fUInts[name] = value; return B_OK; }
    status_t AddInt32(const char* name, int32 value) { fInts[name] = value; return B_OK; }
    status_t FindString(const char* name, BString* string) const {
        auto it = fStrings.find(name);
        if (it != fStrings.end()) { *string = it->second.c_str(); return B_OK; }
        return B_ERROR;
    }
    status_t FindUInt32(const char* name, uint32* value) const {
        auto it = fUInts.find(name);
        if (it != fUInts.end()) { *value = it->second; return B_OK; }
        return B_ERROR;
    }
    status_t FindInt32(const char* name, int32* value) const {
        auto it = fInts.find(name);
        if (it != fInts.end()) { *value = it->second; return B_OK; }
        return B_ERROR;
    }
    status_t Flatten(BFile* file) const { return B_OK; }
    status_t Unflatten(BFile* file) { return B_OK; }
    void MakeEmpty() { fStrings.clear(); fUInts.clear(); fInts.clear(); }

    uint32 what;
private:
    std::map<std::string, std::string> fStrings;
    std::map<std::string, uint32> fUInts;
    std::map<std::string, int32> fInts;
};
#endif
