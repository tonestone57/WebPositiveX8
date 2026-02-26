#ifndef _PATH_H
#define _PATH_H
#include "SupportDefs.h"
#include "String.h"
class BPath {
public:
    BPath() {}
    BPath(const entry_ref* ref) {}
    BPath(const char* path) : fPath(path ? path : "") {}
    status_t SetTo(const char* path) { fPath = path ? path : ""; return B_OK; }
    const char* Path() const { return fPath.String(); }
    const char* Leaf() const { return "test"; }
    status_t GetParent(BPath* path) const { return B_OK; }
    status_t InitCheck() const { return B_OK; }
    status_t Append(const char* path) { fPath << "/" << path; return B_OK; }
private:
    BString fPath;
};
#endif
