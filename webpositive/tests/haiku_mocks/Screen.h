#ifndef _SCREEN_H
#define _SCREEN_H
#include "Rect.h"
class BScreen {
public:
    BRect Frame() { return BRect(0,0,1024,768); }
};
#endif
