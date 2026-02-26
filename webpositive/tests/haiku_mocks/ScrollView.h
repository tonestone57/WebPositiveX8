#ifndef _SCROLL_VIEW_H
#define _SCROLL_VIEW_H
#include "View.h"

class BScrollView : public BView {
public:
    BScrollView(const char* name, BView* target, uint32 resizingMode = 0, bool horizontal = false, bool vertical = false, border_style border = B_FANCY_BORDER) {}
    BScrollView(const char* name, BView* target, uint32 resizingMode, uint32 flags, bool horizontal, bool vertical, border_style border = B_FANCY_BORDER) {}
};
#endif
