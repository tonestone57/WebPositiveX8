#ifndef _GROUP_VIEW_H
#define _GROUP_VIEW_H
#include "View.h"
#include "GroupLayout.h"
class BGroupView : public BView {
public:
    BGroupView(orientation orientation, float spacing = 0.0) : BView(nullptr, 0) {
        BGroupLayout* layout = new BGroupLayout(orientation, spacing);
        SetLayout(layout);
    }
    virtual ~BGroupView() {}
    BGroupLayout* GroupLayout() const { return (BGroupLayout*)GetLayout(); }
    void DoLayout() {}
    void InvalidateLayout(bool descendants = false) {}
};
#endif
