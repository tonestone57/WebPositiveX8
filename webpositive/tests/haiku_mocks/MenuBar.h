#ifndef _MENU_BAR_H
#define _MENU_BAR_H
#include "ListView.h"
#include "MenuItem.h"
enum menu_layout {
    B_ITEMS_IN_ROW,
    B_ITEMS_IN_COLUMN,
    B_ITEMS_IN_MATRIX
};
class BMenu : public BView {
public:
    BMenu(const char* name) {}
    void AddItem(BMenuItem* item) {}
    void AddSeparatorItem() {}
};
class BMenuBar : public BMenu {
public:
    BMenuBar(const char* name) : BMenu(name) {}
};
#endif
