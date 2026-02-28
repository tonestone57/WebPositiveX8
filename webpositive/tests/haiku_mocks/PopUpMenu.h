#ifndef _POP_UP_MENU_H
#define _POP_UP_MENU_H
#include "Menu.h"
class BPopUpMenu : public BMenu {
public:
    BPopUpMenu(const char* title, bool radioMode = true, bool labelFromMarked = true, menu_layout layout = B_ITEMS_IN_COLUMN) : BMenu(title, layout) {}
    BMenuItem* Go(BPoint where, bool deliversMessage = false, bool openAnyway = false, bool asynchronous = false) { return MY_NULLPTR; }
    BMenuItem* Go(BPoint where, bool deliversMessage, bool openAnyway, BRect clickToOpen, bool asynchronous = false) { return MY_NULLPTR; }
};
#endif
