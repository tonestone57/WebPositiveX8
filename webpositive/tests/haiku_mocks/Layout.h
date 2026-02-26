#ifndef _LAYOUT_H
#define _LAYOUT_H

#include "View.h"
#include "InterfaceDefs.h"

class BLayoutItem {
public:
    virtual ~BLayoutItem() {}
    virtual BView* View() { return nullptr; }
    virtual BRect Frame() { return BRect(); }
    virtual void SetVisible(bool visible) {}
    virtual bool IsVisible() { return true; }
    virtual void InvalidateLayout(bool descendants = false) {}
    virtual BSize MinSize() { return BSize(0, 0); }
    virtual BSize MaxSize() { return BSize(10000, 10000); }
    virtual BSize PreferredSize() { return BSize(100, 100); }
};

class BLayout {
public:
    virtual ~BLayout() {}
    virtual BView* View() { return nullptr; }
    virtual int32 IndexOfView(BView* view) { return -1; }
    virtual BLayoutItem* ItemAt(int32 index) { return nullptr; }
    virtual int32 CountItems() const { return 0; }
    virtual BLayoutItem* RemoveItem(int32 index) { return nullptr; }
    virtual void SetInsets(float left, float top, float right, float bottom) {}
    virtual void GetInsets(float* left, float* top, float* right, float* bottom) const { if(left)*left=0; if(top)*top=0; if(right)*right=0; if(bottom)*bottom=0; }
    virtual void InvalidateLayout(bool descendants = false) {}
};

class BTwoDimensionalLayout : public BLayout {};

#endif
