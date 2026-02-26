#ifndef _MENU_ITEM_H
#define _MENU_ITEM_H
#include "SupportDefs.h"
#include "Message.h"
class BMenuItem {
public:
    BMenuItem(const char* label, BMessage* message, char shortcut = 0, uint32 modifiers = 0) {}
    void SetTarget(BMessenger messenger) {}
    void SetEnabled(bool enabled) {}
};
#endif
