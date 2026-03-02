#ifndef _MOCK_LAYOUT_H
#define _MOCK_LAYOUT_H

#include "AbstractLayoutItem.h"
#include <vector>

class BView;

class BLayout : public BLayoutItem {
public:
    BLayout() : fView(nullptr) { SetVisible(true); }
    virtual ~BLayout() {}

    BView* View() const { return fView; }
    void SetView(BView* view) { fView = view; }

    virtual int32 CountItems() const = 0;
    virtual BLayoutItem* ItemAt(int32 index) const = 0;
    virtual BLayoutItem* RemoveItem(int32 index) = 0;

private:
    BView* fView;
};

#endif
