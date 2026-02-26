#ifndef _GROUP_VIEW_H
#define _GROUP_VIEW_H
#include "View.h"
#include "GroupLayout.h"
class BGroupView : public BView {
public:
    BGroupView(orientation orientation, float spacing = 0.0) : BView(nullptr, 0) {
        fLayout = new BGroupLayout(orientation, spacing);
    }
    virtual ~BGroupView() { delete fLayout; }
    BGroupLayout* GroupLayout() const { return fLayout; }
    void DoLayout() {}
    void InvalidateLayout(bool descendants = false) {}
private:
    BGroupLayout* fLayout;
};
#endif
