#ifndef _MENU_ITEM_H
#define _MENU_ITEM_H
#include "SupportDefs.h"
#include "Rect.h"
#include "Message.h"

class BMenu;

class BMenuItem {
public:
    BMenuItem(const char* label, BMessage* message, char shortcut = 0, uint32 modifiers = 0) : fLabel(label ? label : ""), fMessage(message), fSubmenu(nullptr), fMenu(nullptr) {}
    BMenuItem(BMenu* submenu, BMessage* message = nullptr) : fMessage(message), fSubmenu(submenu), fMenu(nullptr) {}
    virtual ~BMenuItem() { delete fMessage; }
    void SetEnabled(bool enabled) {}
    BRect Frame() const { return BRect(0, 0, 10, 10); }
    BMessage* Message() const { return fMessage; }
    void SetMessage(BMessage* message) { delete fMessage; fMessage = message; }
    const char* Label() const { return fLabel.String(); }
    virtual void SetLabel(const char* label) { fLabel = label; }
    void SetMarked(bool marked) {}
    BMenu* Submenu() const { return fSubmenu; }
    BMenu* Menu() const { return fMenu; }
    void SetMenu(BMenu* menu) { fMenu = menu; }
    BView* Parent() const { return nullptr; }

private:
    BString fLabel;
    BMessage* fMessage;
    BMenu* fSubmenu;
    BMenu* fMenu;
};

class BSeparatorItem : public BMenuItem {
public:
    BSeparatorItem() : BMenuItem((const char*)nullptr, nullptr) {}
};
#endif
