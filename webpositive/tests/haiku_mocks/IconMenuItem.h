#ifndef _ICON_MENU_ITEM_H
#define _ICON_MENU_ITEM_H
#include "MenuItem.h"
#include "Bitmap.h"

namespace BPrivate {
class IconMenuItem : public BMenuItem {
public:
    IconMenuItem(const char* label, BMessage* message, BBitmap* icon, int32 which) : BMenuItem(label, message) {}
    IconMenuItem(const char* label, BMessage* message, const char* fileType, int32 which) : BMenuItem(label, message) {}
    IconMenuItem(BMenu* submenu, BMessage* message, const char* fileType, int32 which) : BMenuItem(submenu, message) {}
    IconMenuItem(const char* label, BMessage* message, void* info, int32 which) : BMenuItem(label, message) {}
    virtual ~IconMenuItem() {}
};
}
using BPrivate::IconMenuItem;
#endif
