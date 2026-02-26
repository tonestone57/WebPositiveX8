#ifndef _PROMPT_WINDOW_H
#define _PROMPT_WINDOW_H
#include "Window.h"
class BPromptWindow : public BWindow {
public:
    BPromptWindow(const char* title, const char* label, const char* text, BMessenger target, BMessage* message)
        : BWindow(BRect(), title, B_TITLED_WINDOW, 0) {}
};
#endif
