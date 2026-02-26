#ifndef _DIRECTORY_H
#define _DIRECTORY_H
#include "Node.h"
#include "Entry.h"
class BDirectory : public BNode {
public:
    BDirectory() {}
    BDirectory(const node_ref* ref) {}
    status_t GetNextEntry(BEntry* entry, bool traverse = false) { return B_ERROR; }
};
#endif
