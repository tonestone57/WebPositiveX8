#ifndef _ABSTRACT_LAYOUT_ITEM_H
#define _ABSTRACT_LAYOUT_ITEM_H

#include "Layout.h"

class BAbstractLayoutItem : public BLayoutItem {
public:
    BAbstractLayoutItem() {}
    virtual ~BAbstractLayoutItem() {}

    virtual BSize BaseMinSize() { return BSize(0, 0); }
    virtual BSize BaseMaxSize() { return BSize(10000, 10000); }
    virtual BSize BasePreferredSize() { return BSize(100, 100); }
    virtual BAlignment BaseAlignment() { return BAlignment(); }

    virtual BSize MinSize() override { return BaseMinSize(); }
    virtual BSize MaxSize() override { return BaseMaxSize(); }
    virtual BSize PreferredSize() override { return BasePreferredSize(); }
};

#endif
