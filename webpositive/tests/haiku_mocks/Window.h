#ifndef _WINDOW_H
#define _WINDOW_H
#include "SupportDefs.h"
#include "Message.h"
#include "Rect.h"

enum window_look {
    B_BORDERED_WINDOW_LOOK
};
enum window_feel {
    B_NORMAL_WINDOW_FEEL
};
enum {
    B_NOT_MOVABLE = 1,
    B_WILL_ACCEPT_FIRST_CLICK = 2,
    B_AVOID_FOCUS = 4,
    B_NOT_RESIZABLE = 8,
    B_ASYNCHRONOUS_CONTROLS = 16
};

class BView;

class BWindow {
public:
    BWindow(BRect frame, const char* title, window_look look, window_feel feel, uint32 flags) {}
    virtual ~BWindow() {}
    void Show() {}
    void Hide() {}
    bool IsHidden() { return false; }
    BRect Frame() const { return BRect(0,0,100,100); }
    void Quit() {}
    bool Lock() { return true; }
    void Unlock() {}
    void AddChild(BView* child) {}
    void MoveTo(float x, float y) {}
    void ResizeTo(float w, float h) {}
};
#endif
