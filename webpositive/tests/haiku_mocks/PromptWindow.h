#ifndef _PROMPT_WINDOW_H
#define _PROMPT_WINDOW_H
#include "Window.h"
class BPromptWindow : public BWindow {
public:
    BPromptWindow(const char* title, const char* label, const char* text, BHandler* target, BMessage* message) : BWindow(BRect(), title, B_TITLED_WINDOW, B_NORMAL_WINDOW_FEEL, 0) {}
};
typedef BPromptWindow PromptWindow;
#endif
