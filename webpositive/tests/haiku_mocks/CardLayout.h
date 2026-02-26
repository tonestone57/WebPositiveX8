#ifndef _CARD_LAYOUT_H
#define _CARD_LAYOUT_H
#include "Layout.h"
class BCardLayout : public BLayout {
public:
    BCardLayout() {}
    void SetVisibleItem(int32 index) {}
    int32 VisibleIndex() const { return -1; }
    void AddView(int32 index, BView* view) {}
    virtual int32 CountItems() const { return 0; }
    virtual BLayoutItem* RemoveItem(int32 index) { return nullptr; }
};
#endif
