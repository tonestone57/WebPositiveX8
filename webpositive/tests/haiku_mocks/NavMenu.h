#ifndef _NAV_MENU_H
#define _NAV_MENU_H
#include "PopUpMenu.h"
class BNavMenu : public BPopUpMenu {
public:
    BNavMenu(const char* name, const char* label, BMessenger target, BMessage* message = nullptr)
        : BPopUpMenu(name) {}
};
#endif
