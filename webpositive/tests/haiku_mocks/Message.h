#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "SupportDefs.h"
#include "Rect.h"
#include "String.h"
#include <map>
#include <string>
#include <vector>

class BFile;

class BMessage {
public:
    uint32 what;
    BMessage() : what(0) {}
    BMessage(uint32 what) : what(what) {}
    virtual ~BMessage() {}

    status_t AddString(const char* name, const char* s) { if(!name) return B_BAD_VALUE; fStrings[name].push_back(s ? s : ""); return B_OK; }
    status_t AddString(const char* name, const BString& s) { return AddString(name, s.String()); }
    status_t AddInt32(const char* name, int32 v) { if(!name) return B_BAD_VALUE; fInt32s[name].push_back(v); return B_OK; }
    status_t AddUInt32(const char* name, uint32 v) { if(!name) return B_BAD_VALUE; fInt32s[name].push_back((int32)v); return B_OK; }
    status_t AddInt64(const char* name, int64 v) { if(!name) return B_BAD_VALUE; fInt64s[name].push_back(v); return B_OK; }
    status_t AddBool(const char* name, bool v) { if(!name) return B_BAD_VALUE; fBools[name].push_back(v); return B_OK; }
    status_t AddFloat(const char* name, float v) { if(!name) return B_BAD_VALUE; fFloats[name].push_back(v); return B_OK; }
    status_t AddRect(const char* name, BRect v) { if(!name) return B_BAD_VALUE; fRects[name].push_back(v); return B_OK; }
    status_t AddMessage(const char* name, const BMessage* v) { if(!name) return B_BAD_VALUE; if(v) fMessages[name].push_back(*v); return B_OK; }
    status_t AddPointer(const char* name, const void* v) { if(!name) return B_BAD_VALUE; fPointers[name].push_back(v); return B_OK; }
    status_t AddRef(const char* name, const entry_ref* v) { if(!name) return B_BAD_VALUE; if(v) fRefs[name].push_back(*v); return B_OK; }
    status_t AddData(const char* name, uint32 type, const void* data, ssize_t size) { return B_OK; }

    status_t FindString(const char* name, int32 index, const char** s) const {
        if(!name) return B_BAD_VALUE;
        auto it = fStrings.find(name);
        if(it == fStrings.end() || (size_t)index >= it->second.size()) return B_NAME_NOT_FOUND;
        if(s) *s = it->second[index].c_str();
        return B_OK;
    }
    status_t FindString(const char* name, const char** s) const { return FindString(name, 0, s); }
    status_t FindString(const char* name, BString* s) const {
        const char* str = MY_NULLPTR;
        status_t st = FindString(name, &str);
        if(st == B_OK && s) *s = str;
        return st;
    }
    BString FindString(const char* name, int32 index = 0) const {
        const char* str = MY_NULLPTR;
        FindString(name, index, &str);
        return BString(str);
    }

    template<typename T>
    status_t FindInt32(const char* name, T* v) const { return FindInt(name, 0, v); }
    status_t FindInt32(const char* name, int32 index, int32* v) const { return FindInt(name, index, v); }
    template<typename T>
    status_t FindUInt32(const char* name, T* v) const { return FindInt(name, 0, v); }

    template<typename T>
    status_t FindInt64(const char* name, T* v) const { return FindInt(name, 0, v); }
    status_t FindInt64(const char* name, int32 index, int64* v) const { return FindInt(name, index, v); }

    int64 FindInt64(const char* name) const { int64 v = 0; FindInt(name, 0, &v); return v; }

    status_t FindBool(const char* name, int32 index, bool* v) const { return FindValue(fBools, name, index, v); }
    status_t FindBool(const char* name, bool* v) const { return FindBool(name, 0, v); }
    status_t FindFloat(const char* name, int32 index, float* v) const { return FindValue(fFloats, name, index, v); }
    status_t FindFloat(const char* name, float* v) const { return FindFloat(name, 0, v); }
    status_t FindRect(const char* name, int32 index, BRect* v) const { return FindValue(fRects, name, index, v); }
    status_t FindRect(const char* name, BRect* v) const { return FindRect(name, 0, v); }
    status_t FindMessage(const char* name, int32 index, BMessage* v) const { return FindValue(fMessages, name, index, v); }
    status_t FindMessage(const char* name, BMessage* v) const { return FindMessage(name, 0, v); }
    status_t FindPointer(const char* name, int32 index, void** v) const { return FindValue(fPointers, name, index, v); }
    status_t FindPointer(const char* name, void** v) const { return FindPointer(name, 0, v); }
    status_t FindRef(const char* name, int32 index, entry_ref* v) const { return FindValue(fRefs, name, index, v); }
    status_t FindRef(const char* name, entry_ref* v) const { return FindRef(name, 0, v); }
    status_t FindData(const char* name, uint32 type, const void** data, ssize_t* size) const { return B_NAME_NOT_FOUND; }

    void MakeEmpty() {
        fStrings.clear(); fInt32s.clear(); fInt64s.clear(); fBools.clear();
        fFloats.clear(); fRects.clear(); fMessages.clear(); fPointers.clear(); fRefs.clear();
    }
    status_t Flatten(BFile* file) const { return B_OK; }
    status_t Unflatten(BFile* file) { return B_OK; }

private:
    template<typename M, typename T>
    status_t FindValue(const M& map, const char* name, int32 index, T* value) const {
        if(!name) return B_BAD_VALUE;
        auto it = map.find(name);
        if(it == map.end() || (size_t)index >= it->second.size()) return B_NAME_NOT_FOUND;
        if(value) *value = (T)it->second[index];
        return B_OK;
    }

    template<typename T>
    status_t FindInt(const char* name, int32 index, T* value) const {
        if(!name) return B_BAD_VALUE;
        auto it32 = fInt32s.find(name);
        if(it32 != fInt32s.end() && (size_t)index < it32->second.size()) {
            if(value) *value = (T)it32->second[index];
            return B_OK;
        }
        auto it64 = fInt64s.find(name);
        if(it64 != fInt64s.end() && (size_t)index < it64->second.size()) {
            if(value) *value = (T)it64->second[index];
            return B_OK;
        }
        return B_NAME_NOT_FOUND;
    }

    std::map<std::string, std::vector<std::string>> fStrings;
    std::map<std::string, std::vector<int32>> fInt32s;
    std::map<std::string, std::vector<int64>> fInt64s;
    std::map<std::string, std::vector<bool>> fBools;
    std::map<std::string, std::vector<float>> fFloats;
    std::map<std::string, std::vector<BRect>> fRects;
    std::map<std::string, std::vector<BMessage>> fMessages;
    std::map<std::string, std::vector<const void*>> fPointers;
    std::map<std::string, std::vector<entry_ref>> fRefs;
};

#endif
