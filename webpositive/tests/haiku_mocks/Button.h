#ifndef _BUTTON_H
#define _BUTTON_H
#include "ListView.h"
class BButton : public BView {
public:
    BButton(const char* label, BMessage* message) {}
    BButton(BRect frame, const char* name, const char* label, BMessage* message, uint32 resizingMode = 0, uint32 flags = 0) {}
};
#endif
