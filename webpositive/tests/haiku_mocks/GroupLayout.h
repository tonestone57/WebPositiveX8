#ifndef _MOCK_GROUP_LAYOUT_H
#define _MOCK_GROUP_LAYOUT_H
#include "Layout.h"
#include "InterfaceDefs.h"
class BGroupLayout : public BLayout {
public:
    BGroupLayout(orientation orientation, float spacing = 0.0f) {}
    void AddView(int32 index, BView* view) {}
    void AddView(BView* view) {}
    void AddView(BView* view, float weight) {}
    void AddItem(BLayoutItem* item) {}
    void AddItem(int32 index, BLayoutItem* item) {}
    void AddItem(BLayoutItem* item, float weight) {}
    int32 IndexOfItem(BLayoutItem* item) const { return -1; }
    virtual int32 CountItems() const { return 0; }
    virtual BLayoutItem* RemoveItem(int32 index) { return nullptr; }
    virtual void SetInsets(float left, float top, float right, float bottom) {}
    virtual void GetInsets(float* left, float* top, float* right, float* bottom) const { if(left)*left=0; if(top)*top=0; if(right)*right=0; if(bottom)*bottom=0; }
};
#endif
