#ifndef _MENU_BAR_H
#define _MENU_BAR_H
#include "Menu.h"
class BMenuBar : public BMenu {
public:
    BMenuBar(const char* title) : BMenu(title) {}
    BMenuBar(BRect frame, const char* title, uint32 resizingMode, menu_layout layout, bool resizeToFit) : BMenu(title, layout) {}
};
#endif
