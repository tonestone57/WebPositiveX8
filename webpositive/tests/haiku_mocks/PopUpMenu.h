#ifndef _POP_UP_MENU_H
#define _POP_UP_MENU_H
#include "MenuBar.h"
class BPopUpMenu : public BMenu {
public:
    BPopUpMenu(const char* name, bool radioMode = true, bool autoRename = true, menu_layout layout = B_ITEMS_IN_COLUMN)
        : BMenu(name) {}
};
#endif
