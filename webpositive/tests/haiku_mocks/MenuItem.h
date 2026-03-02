#ifndef _MENU_ITEM_H
#define _MENU_ITEM_H
#include "SupportDefs.h"
#include "Rect.h"
#include "Message.h"

class BMenu;

class BMenuItem {
public:
    BMenuItem(const char* label, BMessage* message, char shortcut = 0, uint32 modifiers = 0) : fLabel(label ? label : ""), fMessage(message), fSubmenu(MY_NULLPTR) {}
    BMenuItem(BMenu* submenu, BMessage* message = MY_NULLPTR) : fMessage(message), fSubmenu(submenu) {}
    virtual ~BMenuItem() { delete fMessage; }
    void SetEnabled(bool enabled) {}
    BRect Frame() const { return BRect(0, 0, 10, 10); }
    BMessage* Message() const { return fMessage; }
    void SetMessage(BMessage* message) { delete fMessage; fMessage = message; }
    const char* Label() const { return fLabel.String(); }
    virtual void SetLabel(const char* label) { fLabel = label; }
    void SetMarked(bool marked) {}
    BMenu* Submenu() const { return fSubmenu; }

private:
    BString fLabel;
    BMessage* fMessage;
    BMenu* fSubmenu;
};

class BSeparatorItem : public BMenuItem {
public:
    BSeparatorItem() : BMenuItem((const char*)MY_NULLPTR, MY_NULLPTR) {}
};
#endif
