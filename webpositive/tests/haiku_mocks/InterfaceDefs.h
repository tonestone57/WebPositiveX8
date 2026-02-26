#ifndef _INTERFACE_DEFS_H
#define _INTERFACE_DEFS_H
#include "SupportDefs.h"
#include <cmath>

typedef enum {
    B_OP_COPY,
    B_OP_ALPHA,
    B_SOLID_LOW,
    B_SOLID_HIGH
} drawing_mode;

enum color_which {
    B_PANEL_BACKGROUND_COLOR,
    B_LIST_BACKGROUND_COLOR
};

enum orientation {
    B_HORIZONTAL,
    B_VERTICAL
};

enum alignment {
    B_ALIGN_LEFT,
    B_ALIGN_RIGHT,
    B_ALIGN_CENTER,
    B_ALIGN_MIDDLE = B_ALIGN_CENTER,
    B_ALIGN_USE_FULL_WIDTH = B_ALIGN_CENTER,
    B_ALIGN_USE_FULL_HEIGHT = B_ALIGN_CENTER
};

struct BAlignment {
    alignment horizontal;
    alignment vertical;
    BAlignment(alignment h, alignment v) : horizontal(h), vertical(v) {}
};

struct font_height {
    float ascent;
    float descent;
    float leading;
};
#define B_UTF8_ELLIPSIS "\xE2\x80\xA6"

enum color_space {
    B_RGBA32,
    B_RGB32,
    B_CMAP8
};

#define B_DARKEN_1_TINT 1.0f
#define B_DARKEN_4_TINT 1.5f
#define B_NO_TINT 0.0f
#define B_V_SCROLL_BAR_WIDTH 15.0f

enum {
    B_LIST_SELECTED_ITEM_TEXT_COLOR,
    B_LIST_SELECTED_BACKGROUND_COLOR,
    B_LIST_ITEM_TEXT_COLOR,
    B_PANEL_TEXT_COLOR
};

inline rgb_color ui_color(color_which color) { return {200, 200, 200, 255}; }
inline rgb_color ui_color(int color) { return {200, 200, 200, 255}; }
inline rgb_color tint_color(rgb_color color, float tint) { return color; }

enum {
    B_TRANSPARENT_COLOR = 0,
    B_ALPHA_OVERLAY = 0,
    B_FILTER_BITMAP_BILINEAR = 0
};
typedef enum {
    B_PIXEL_ALPHA
} source_alpha;

enum {
    B_CONTROL_ON = 1,
    B_CONTROL_OFF = 0
};

enum {
    B_INSIDE_VIEW,
    B_OUTSIDE_VIEW,
    B_ENTERED_VIEW,
    B_EXITED_VIEW
};

#define B_COPY 'copy'
#define B_USE_SMALL_SPACING 5.0f
#define B_USE_DEFAULT_SPACING 10.0f
#define B_MIME_TYPE 'mime'

#endif
