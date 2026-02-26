#ifndef _CARD_LAYOUT_H
#define _CARD_LAYOUT_H
#include "Layout.h"
#include <vector>

class BCardLayout : public BLayout {
public:
    BCardLayout() : fVisibleIndex(-1) {}
    virtual ~BCardLayout() {
        for (auto item : fItems) delete item;
    }
    void SetVisibleItem(int32 index) { fVisibleIndex = index; }
    int32 VisibleIndex() const { return fVisibleIndex; }
    void AddView(int32 index, BView* view) {
        if (!view) return;
        BLayoutItem* item = new BLayoutItem();
        item->SetView(view);
        AddItem(index, item);
        if (View()) View()->AddChild(view);
    }
    void AddItem(int32 index, BLayoutItem* item) {
        if (!item) return;
        item->SetLayout(this);
        if (index < 0 || index >= (int32)fItems.size())
            fItems.push_back(item);
        else
            fItems.insert(fItems.begin() + index, item);

        if (View() && item->View()) View()->AddChild(item->View());
    }
    virtual int32 CountItems() const { return (int32)fItems.size(); }
    virtual BLayoutItem* ItemAt(int32 index) const {
        if (index < 0 || index >= (int32)fItems.size()) return nullptr;
        return fItems[index];
    }
    virtual BLayoutItem* RemoveItem(int32 index) {
        if (index < 0 || index >= (int32)fItems.size()) return nullptr;
        BLayoutItem* item = fItems[index];
        fItems.erase(fItems.begin() + index);
        if (fVisibleIndex >= (int32)fItems.size()) fVisibleIndex = (int32)fItems.size() - 1;
        return item;
    }

private:
    std::vector<BLayoutItem*> fItems;
    int32 fVisibleIndex;
};
#endif
