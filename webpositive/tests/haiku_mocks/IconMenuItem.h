#ifndef _ICON_MENU_ITEM_H
#define _ICON_MENU_ITEM_H
#include "MenuItem.h"
class BIconMenuItem : public BMenuItem {
public:
    BIconMenuItem(const char* label, BMessage* message, char shortcut = 0, uint32 modifiers = 0, const BBitmap* icon = nullptr)
        : BMenuItem(label, message, shortcut, modifiers) {}
};
#endif
