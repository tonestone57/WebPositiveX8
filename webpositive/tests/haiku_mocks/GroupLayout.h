#ifndef _MOCK_GROUP_LAYOUT_H
#define _MOCK_GROUP_LAYOUT_H
#include "Layout.h"
#include "InterfaceDefs.h"
#include <vector>

class BGroupLayout : public BLayout {
public:
    BGroupLayout(orientation orientation, float spacing = 0.0f) {}
    virtual ~BGroupLayout() {
        for (auto item : fItems) delete item;
    }
    void AddView(int32 index, BView* view) {
        if (!view) return;
        BLayoutItem* item = new BLayoutItem();
        item->SetView(view);
        AddItem(index, item);
        if (View()) View()->AddChild(view);
    }
    void AddView(BView* view) { AddView(-1, view); }
    void AddView(BView* view, float weight) { AddView(-1, view); }
    void AddItem(BLayoutItem* item) { AddItem(-1, item); }
    void AddItem(int32 index, BLayoutItem* item) {
        if (!item) return;
        item->SetLayout(this);
        if (index < 0 || index >= (int32)fItems.size())
            fItems.push_back(item);
        else
            fItems.insert(fItems.begin() + index, item);

        if (View() && item->View()) View()->AddChild(item->View());
    }
    void AddItem(BLayoutItem* item, float weight) { AddItem(-1, item); }
    int32 IndexOfItem(BLayoutItem* item) const {
        for(size_t i=0; i<fItems.size(); ++i) if(fItems[i] == item) return (int32)i;
        return -1;
    }
    virtual int32 CountItems() const { return (int32)fItems.size(); }
    virtual BLayoutItem* ItemAt(int32 index) const {
        if (index < 0 || index >= (int32)fItems.size()) return MY_NULLPTR;
        return fItems[index];
    }
    virtual BLayoutItem* RemoveItem(int32 index) {
        if (index < 0 || index >= (int32)fItems.size()) return MY_NULLPTR;
        BLayoutItem* item = fItems[index];
        fItems.erase(fItems.begin() + index);
        // NOTE: In Haiku, removing from layout does NOT remove from view hierarchy.
        return item;
    }
    virtual void SetInsets(float left, float top, float right, float bottom) {}
    virtual void GetInsets(float* left, float* top, float* right, float* bottom) const { if(left)*left=0; if(top)*top=0; if(right)*right=0; if(bottom)*bottom=0; }

    virtual BSize MinSize() override {
        BSize size(0, 0);
        for(auto item : fItems) {
            BSize itemMin = item->MinSize();
            size.width += itemMin.width;
            if (itemMin.height > size.height) size.height = itemMin.height;
        }
        return size;
    }

private:
    std::vector<BLayoutItem*> fItems;
};
#endif
