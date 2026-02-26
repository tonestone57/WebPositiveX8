#ifndef _FONT_H
#define _FONT_H
#include "InterfaceDefs.h"
class BFont {
public:
    void GetHeight(font_height* height) const {
        if (height) { height->ascent = 10; height->descent = 2; height->leading = 1; }
    }
};
#endif
