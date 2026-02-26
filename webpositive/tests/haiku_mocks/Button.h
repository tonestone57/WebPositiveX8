#ifndef _BUTTON_H
#define _BUTTON_H
#include "View.h"
class BButton : public BView {
public:
    BButton(const char* label, BMessage* message) {}
    BButton(const char* name, const char* label, BMessage* message) {}
    BButton(BRect frame, const char* name, const char* label, BMessage* message, uint32 resizingMode = 0, uint32 flags = 0) {}
    virtual void SetEnabled(bool enabled) {}
    virtual void SetValue(int32 value) {}
    virtual int32 Value() const { return 0; }
    virtual void Invoke(BMessage* message = nullptr) {}
};
#endif
