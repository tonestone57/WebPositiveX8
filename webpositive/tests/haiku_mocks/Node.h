#ifndef _NODE_H
#define _NODE_H
#include "SupportDefs.h"
class BEntry;
class BNode {
public:
    BNode() {}
    BNode(const BEntry* entry) {}
    virtual ~BNode() {}
    status_t InitCheck() const { return B_OK; }
};
#endif
