#ifndef _TEXT_CONTROL_H
#define _TEXT_CONTROL_H
#include "ListView.h"
class BTextControl : public BView {
public:
    BTextControl(BRect frame, const char* name, const char* label, const char* text, BMessage* message, uint32 resizingMode = 0, uint32 flags = 0) {}
    BTextControl(const char* label, const char* text, BMessage* message) {}
    void SetText(const char* text) {}
    const char* Text() const { return ""; }
};
#endif
