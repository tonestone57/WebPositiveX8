#ifndef _NAV_MENU_H
#define _NAV_MENU_H
#include "Menu.h"
class BNavMenu : public BMenu {
public:
    BNavMenu(const char* name, uint32 what, BHandler* target) : BMenu(name) {}
    BNavMenu(const char* name, uint32 what, BWindow* target) : BMenu(name) {}
    void SetNavDir(const entry_ref* ref) {}
};
#endif
