#ifndef _WEB_WINDOW_H
#define _WEB_WINDOW_H
#include "Window.h"
class BWebWindow : public BWindow {
public:
    BWebWindow(BRect frame, const char* title, window_type type, uint32 flags, uint32 workspaces = B_CURRENT_WORKSPACE)
        : BWindow(frame, title, type, flags, workspaces) {}
};
#endif
