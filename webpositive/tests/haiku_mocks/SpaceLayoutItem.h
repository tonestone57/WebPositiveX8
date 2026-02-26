#ifndef _SPACE_LAYOUT_ITEM_H
#define _SPACE_LAYOUT_ITEM_H
#include "Layout.h"
class BSpaceLayoutItem : public BLayoutItem {
public:
    static BSpaceLayoutItem* CreateGlue() { return new BSpaceLayoutItem(); }
    static BSpaceLayoutItem* CreateHorizontalStrut(float width) { return new BSpaceLayoutItem(); }
    static BSpaceLayoutItem* CreateVerticalStrut(float height) { return new BSpaceLayoutItem(); }
};
#endif
