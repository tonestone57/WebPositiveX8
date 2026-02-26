#ifndef _MENU_BAR_H
#define _MENU_BAR_H
#include "Menu.h"
class BMenuBar : public BMenu {
public:
    BMenuBar(const char* title) : BMenu(title) {
        SetFlags(Flags() | B_FRAME_EVENTS); // Matches original code usage if needed
    }
    BMenuBar(BRect frame, const char* title, uint32 resizingMode, menu_layout layout, bool resizeToFit) : BMenu(title, layout) {}
};
#endif
