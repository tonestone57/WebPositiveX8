#ifndef _MESSAGE_H
#define _MESSAGE_H
#include "SupportDefs.h"
#include "String.h"
class BFile;
class BMessage {
public:
    BMessage() : what(0) {}
    BMessage(uint32 what) : what(what) {}
    status_t FindMessage(const char* name, int32 index, BMessage* message) const { return B_ERROR; }
    status_t FindMessage(const char* name, BMessage* message) const { return B_ERROR; }
    status_t AddMessage(const char* name, const BMessage* message) { return B_OK; }
    status_t AddString(const char* name, const char* string) { return B_OK; }
    status_t AddUInt32(const char* name, uint32 value) { return B_OK; }
    status_t AddInt32(const char* name, int32 value) { return B_OK; }
    status_t FindString(const char* name, BString* string) const { return B_ERROR; }
    status_t FindUInt32(const char* name, uint32* value) const { return B_ERROR; }
    status_t FindInt32(const char* name, int32* value) const { return B_ERROR; }
    status_t Flatten(BFile* file) const { return B_OK; }
    status_t Unflatten(BFile* file) { return B_OK; }
    void MakeEmpty() {}

    uint32 what;
};
#endif
