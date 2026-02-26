#ifndef _ENTRY_H
#define _ENTRY_H
#include "SupportDefs.h"
#include "Path.h"
class BEntry {
public:
    BEntry() {}
    BEntry(const char* path) {}
    BEntry(const entry_ref* ref, bool traverse = false) {}
    virtual ~BEntry() {}
    status_t InitCheck() const { return B_OK; }
    bool Exists() const { return true; }
    bool IsDirectory() const { return false; }
    status_t GetPath(BPath* path) const;
    status_t GetRef(entry_ref* ref) const { return B_OK; }
    status_t GetName(char* buffer) const { if (buffer) strcpy(buffer, "test"); return B_OK; }
    status_t GetNodeRef(node_ref* ref) const { return B_OK; }
    status_t Rename(const char* name, bool clobber = false) { return B_OK; }
    status_t Remove() { return B_OK; }
    status_t GetParent(BEntry* entry) const { return B_OK; }
};
#endif
