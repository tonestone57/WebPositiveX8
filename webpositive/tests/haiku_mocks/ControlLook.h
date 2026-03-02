#ifndef _CONTROL_LOOK_H
#define _CONTROL_LOOK_H
#include "InterfaceDefs.h"

class BView;

class BControlLook {
public:
    BControlLook();
    float DefaultItemSpacing() const { return 5.0f; }
    float DefaultLabelSpacing() const { return 5.0f; }
    float ComposeIconSize(uint32 size) const { return (float)size; }
    void DrawButtonFrame(BView* owner, BRect& rect, const BRect& updateRect, rgb_color base, rgb_color background, uint32 flags = 0) {}
    void DrawButtonBackground(BView* owner, BRect& rect, const BRect& updateRect, rgb_color base, uint32 flags = 0, uint32 borders = 0xFFFFFFFF) {}
    void DrawArrowShape(BView* owner, BRect& rect, const BRect& updateRect, rgb_color base, uint32 direction, uint32 flags = 0, float tint = B_DARKEN_1_TINT) {}
    void DrawTabFrame(BView* owner, BRect& rect, const BRect& updateRect, rgb_color base, uint32 borders = 0, uint32 flags = 0, uint32 side = 0) {}
    void DrawActiveTab(BView* owner, BRect& rect, const BRect& updateRect, rgb_color base, uint32 flags = 0, uint32 borders = 0, uint32 side = 0, int32 index = 0, int32 selected = -1, int32 first = 0, int32 last = 0) {}
    void DrawInactiveTab(BView* owner, BRect& rect, const BRect& updateRect, rgb_color base, uint32 flags = 0, uint32 borders = 0, uint32 side = 0, int32 index = 0, int32 selected = -1, int32 first = 0, int32 last = 0) {}
    void DrawLabel(BView* owner, const char* label, BRect rect, const BRect& updateRect, rgb_color base, uint32 flags, const BAlignment& alignment, const rgb_color* textColor = MY_NULLPTR) {}

    uint32 Flags(BView* view) const { return 0; }

    enum {
        B_ACTIVATED = 1,
        B_BLEND_FRAME = 2
    };

    enum {
        B_LEFT_ARROW,
        B_RIGHT_ARROW,
        B_UP_ARROW,
        B_DOWN_ARROW
    };

    enum {
        B_TOP_BORDER = 1,
        B_BOTTOM_BORDER = 2,
        B_LEFT_BORDER = 4,
        B_RIGHT_BORDER = 8
    };
};
extern BControlLook* be_control_look;
#endif
