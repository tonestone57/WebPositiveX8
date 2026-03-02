#ifndef _MOCK_ABSTRACT_LAYOUT_ITEM_H
#define _MOCK_ABSTRACT_LAYOUT_ITEM_H

#include "Rect.h"
#include "Size.h"

class BView;

#include "InterfaceDefs.h"

class BLayoutItem {
public:
    BLayoutItem() : fView(nullptr), fVisible(true) {}
    virtual ~BLayoutItem() {}

    virtual BRect Frame() const { return BRect(); }
    virtual void SetVisible(bool visible) { fVisible = visible; }
    virtual bool IsVisible() const { return fVisible; }

    virtual BSize MinSize() { return BSize(); }
    virtual BSize MaxSize() { return BSize(); }
    virtual BSize PreferredSize() { return BSize(); }

    virtual BView* View() const { return fView; }
    virtual void SetView(BView* view) { fView = view; }
    virtual void SetLayout(void* layout) {}

    virtual void InvalidateLayout() {}

private:
    BView* fView;
    bool fVisible;
};

class BAbstractLayoutItem : public BLayoutItem {
public:
    BAbstractLayoutItem() : BLayoutItem() {}
    virtual ~BAbstractLayoutItem() {}

    virtual BAlignment BaseAlignment() { return BAlignment(); }
    virtual BSize BaseMinSize() { return MinSize(); }
    virtual BSize BaseMaxSize() { return MaxSize(); }
    virtual BSize BasePreferredSize() { return PreferredSize(); }
};

#endif
