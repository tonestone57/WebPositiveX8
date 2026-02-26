#ifndef _INTERFACE_DEFS_H
#define _INTERFACE_DEFS_H
#include "SupportDefs.h"

enum color_which {
    B_LIST_SELECTED_ITEM_TEXT_COLOR,
    B_LIST_SELECTED_BACKGROUND_COLOR,
    B_LIST_ITEM_TEXT_COLOR,
    B_LIST_BACKGROUND_COLOR,
    B_CONTROL_HIGHLIGHT_COLOR
};

inline rgb_color ui_color(color_which which) {
    rgb_color c = {0,0,0,255};
    return c;
}

struct font_height {
    float ascent, descent, leading;
};

#define B_NO_TINT 1.0f
#define B_DARKEN_1_TINT 1.1f
inline rgb_color tint_color(rgb_color c, float tint) { return c; }

enum drawing_mode {
    B_OP_COPY,
    B_SOLID_LOW
};

enum orientation {
    B_HORIZONTAL = 0,
    B_VERTICAL = 1
};

enum {
    B_SECONDARY_MOUSE_BUTTON = 2
};

enum {
    B_COPY = 'copy',
    B_PASTE = 'pst ',
    B_CUT = 'cut ',
    B_SELECT_ALL = 'sall'
};

#define B_V_SCROLL_BAR_WIDTH 15.0f

#endif
