#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "SupportDefs.h"
#include "Rect.h"
#include "String.h"
#include <map>
#include <vector>
#include <string>

class BHandler;
class BFile;

class BMessage {
public:
    BMessage() : what(0) {}
    BMessage(uint32 what) : what(what) {}
    virtual ~BMessage() {}
    uint32 what;

    status_t AddString(const char* name, const char* string) { if (!name) return B_BAD_VALUE; fStrings[name] = string ? string : ""; return B_OK; }
    status_t AddString(const char* name, const BString& string) { return AddString(name, string.String()); }
    status_t AddInt32(const char* name, int32 value) { if (!name) return B_BAD_VALUE; fInt32s[name] = value; return B_OK; }
    status_t AddUInt32(const char* name, uint32 value) { if (!name) return B_BAD_VALUE; fUInt32s[name] = value; return B_OK; }
    status_t AddInt64(const char* name, int64 value) { if (!name) return B_BAD_VALUE; fInt64s[name] = value; return B_OK; }
    status_t AddBool(const char* name, bool value) { if (!name) return B_BAD_VALUE; fBools[name] = value; return B_OK; }
    status_t AddFloat(const char* name, float value) { if (!name) return B_BAD_VALUE; fFloats[name] = value; return B_OK; }
    status_t AddRect(const char* name, BRect rect) { if (!name) return B_BAD_VALUE; fRects[name] = rect; return B_OK; }
    status_t AddMessage(const char* name, const BMessage* message) { if (!name) return B_BAD_VALUE; if (message) fMessages[name] = *message; return B_OK; }
    status_t AddPointer(const char* name, const void* pointer) { if (!name) return B_BAD_VALUE; fPointers[name] = pointer; return B_OK; }
    status_t AddRef(const char* name, const entry_ref* ref) { if (!name) return B_BAD_VALUE; if (ref) fRefs[name] = *ref; return B_OK; }
    status_t AddData(const char* name, uint32 type, const void* data, ssize_t size) { return B_OK; }

    status_t FindString(const char* name, BString* string) const {
        if (!name) return B_BAD_VALUE;
        auto it = fStrings.find(name);
        if (it != fStrings.end()) { if (string) *string = it->second.c_str(); return B_OK; }
        return B_NAME_NOT_FOUND;
    }
    BString FindString(const char* name) const {
        if (!name) return BString();
        auto it = fStrings.find(name);
        if (it != fStrings.end()) return BString(it->second.c_str());
        return BString();
    }
    status_t FindString(const char* name, int32 index, BString* string) const { return FindString(name, string); }
    status_t FindString(const char* name, const char** string) const {
        if (!name) return B_BAD_VALUE;
        auto it = fStrings.find(name);
        if (it != fStrings.end()) { if (string) *string = it->second.c_str(); return B_OK; }
        return B_NAME_NOT_FOUND;
    }
    status_t FindInt32(const char* name, int32* value) const {
        if (!name) return B_BAD_VALUE;
        auto it = fInt32s.find(name);
        if (it != fInt32s.end()) { if (value) *value = it->second; return B_OK; }
        return B_NAME_NOT_FOUND;
    }
    status_t FindInt32(const char* name, dev_t* value) const {
        if (!name) return B_BAD_VALUE;
        auto it = fInt32s.find(name);
        if (it != fInt32s.end()) { if (value) *value = (dev_t)it->second; return B_OK; }
        return B_NAME_NOT_FOUND;
    }
    status_t FindInt32(const char* name, int32 index, int32* value) const { return FindInt32(name, value); }
    status_t FindUInt32(const char* name, uint32* value) const {
        if (!name) return B_BAD_VALUE;
        auto it = fUInt32s.find(name);
        if (it != fUInt32s.end()) { if (value) *value = it->second; return B_OK; }
        return B_NAME_NOT_FOUND;
    }
    status_t FindInt64(const char* name, int64* value) const {
        if (!name) return B_BAD_VALUE;
        auto it = fInt64s.find(name);
        if (it != fInt64s.end()) { if (value) *value = it->second; return B_OK; }
        return B_NAME_NOT_FOUND;
    }
    int64 FindInt64(const char* name) const {
        if (!name) return 0;
        auto it = fInt64s.find(name);
        if (it != fInt64s.end()) return it->second;
        return 0;
    }
    status_t FindInt64(const char* name, uint64* value) const {
        if (!name) return B_BAD_VALUE;
        auto it = fInt64s.find(name);
        if (it != fInt64s.end()) { if (value) *value = (uint64)it->second; return B_OK; }
        return B_NAME_NOT_FOUND;
    }
    status_t FindBool(const char* name, bool* value) const {
        if (!name) return B_BAD_VALUE;
        auto it = fBools.find(name);
        if (it != fBools.end()) { if (value) *value = it->second; return B_OK; }
        return B_NAME_NOT_FOUND;
    }
    status_t FindFloat(const char* name, float* value) const {
        if (!name) return B_BAD_VALUE;
        auto it = fFloats.find(name);
        if (it != fFloats.end()) { if (value) *value = it->second; return B_OK; }
        return B_NAME_NOT_FOUND;
    }
    status_t FindRect(const char* name, BRect* rect) const {
        if (!name) return B_BAD_VALUE;
        auto it = fRects.find(name);
        if (it != fRects.end()) { if (rect) *rect = it->second; return B_OK; }
        return B_NAME_NOT_FOUND;
    }
    status_t FindMessage(const char* name, BMessage* message) const {
        if (!name) return B_BAD_VALUE;
        auto it = fMessages.find(name);
        if (it != fMessages.end()) { if (message) *message = it->second; return B_OK; }
        return B_NAME_NOT_FOUND;
    }
    status_t FindMessage(const char* name, int32 index, BMessage* message) const { return FindMessage(name, message); }
    status_t FindPointer(const char* name, void** pointer) const {
        if (!name) return B_BAD_VALUE;
        auto it = fPointers.find(name);
        if (it != fPointers.end()) { if (pointer) *pointer = (void*)it->second; return B_OK; }
        return B_NAME_NOT_FOUND;
    }
    status_t FindPointer(const char* name, int32 index, void** pointer) const { return FindPointer(name, pointer); }
    status_t FindRef(const char* name, entry_ref* ref) const {
        if (!name) return B_BAD_VALUE;
        auto it = fRefs.find(name);
        if (it != fRefs.end()) { if (ref) *ref = it->second; return B_OK; }
        return B_NAME_NOT_FOUND;
    }
    status_t FindRef(const char* name, int32 index, entry_ref* ref) const { return FindRef(name, ref); }
    status_t FindData(const char* name, uint32 type, const void** data, ssize_t* size) const { return B_NAME_NOT_FOUND; }

    void MakeEmpty() {
        fStrings.clear(); fInt32s.clear(); fUInt32s.clear(); fInt64s.clear();
        fBools.clear(); fFloats.clear(); fRects.clear(); fMessages.clear();
        fPointers.clear(); fRefs.clear();
    }
    status_t Flatten(BFile* file) const { return B_OK; }
    status_t Unflatten(BFile* file) { return B_OK; }

private:
    std::map<std::string, std::string> fStrings;
    std::map<std::string, int32> fInt32s;
    std::map<std::string, uint32> fUInt32s;
    std::map<std::string, int64> fInt64s;
    std::map<std::string, bool> fBools;
    std::map<std::string, float> fFloats;
    std::map<std::string, BRect> fRects;
    std::map<std::string, BMessage> fMessages;
    std::map<std::string, const void*> fPointers;
    std::map<std::string, entry_ref> fRefs;
};

#endif
