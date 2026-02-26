#ifndef _ENTRY_H
#define _ENTRY_H
#include "SupportDefs.h"
#include "Path.h"
#include <string.h>

class BEntry {
public:
    BEntry() : fIsDirectory(false), fRef(0, 0, nullptr) {}
    BEntry(const char* path) : fIsDirectory(false), fRef(0, 0, path) {}
    BEntry(const entry_ref* ref, bool traverse = false) : fIsDirectory(false) {
        if (ref) fRef = *ref;
    }
    virtual ~BEntry() {}
    status_t InitCheck() const { return B_OK; }
    bool Exists() const { return true; }
    bool IsDirectory() const { return fIsDirectory; }
    void SetIsDirectory(bool isDir) { fIsDirectory = isDir; }
    status_t GetPath(BPath* path) const {
        if (path && fRef.name) path->SetTo(fRef.name);
        return B_OK;
    }
    status_t GetRef(entry_ref* ref) const {
        if (ref) *ref = fRef;
        return B_OK;
    }
    status_t GetName(char* buffer) const {
        if (buffer) {
            if (fRef.name) strcpy(buffer, fRef.name);
            else strcpy(buffer, "");
        }
        return B_OK;
    }
    status_t GetNodeRef(node_ref* ref) const {
        if (ref) {
            ref->device = fRef.device;
            ref->node = (ino_t)fRef.directory; // Mock logic
        }
        return B_OK;
    }
    status_t Rename(const char* name, bool clobber = false) { return B_OK; }
    status_t Remove() { return B_OK; }
    status_t GetParent(BEntry* entry) const { return B_OK; }
private:
    bool fIsDirectory;
    entry_ref fRef;
};
#endif
