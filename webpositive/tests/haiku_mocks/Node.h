#ifndef _NODE_H
#define _NODE_H
#include "SupportDefs.h"
#include <sys/types.h>
struct node_ref {
    dev_t device;
    ino_t node;
};
class BNode {
public:
    BNode() {}
};
#endif
