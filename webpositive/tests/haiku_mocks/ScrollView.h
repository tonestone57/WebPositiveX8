#ifndef _SCROLL_VIEW_H
#define _SCROLL_VIEW_H
#include "ListView.h"
class BScrollView : public BView {
public:
    BScrollView(const char* name, BView* target, uint32 resizingMode, uint32 flags, bool horizontal, bool vertical, uint32 border = 0) {}
    BScrollView(const char* name, BView* target, uint32 flags, bool horizontal, bool vertical, uint32 border = 0) {}
    virtual ~BScrollView() {}
};
#endif
