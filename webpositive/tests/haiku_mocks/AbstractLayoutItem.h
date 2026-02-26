#ifndef _ABSTRACT_LAYOUT_ITEM_H
#define _ABSTRACT_LAYOUT_ITEM_H
#include "Layout.h"
#include "InterfaceDefs.h"
class BAbstractLayoutItem : public BLayoutItem {
public:
    virtual BAlignment BaseAlignment() { return BAlignment(B_ALIGN_LEFT, B_ALIGN_CENTER); }
};
#endif
