#ifndef _ALERT_H
#define _ALERT_H
#include "Window.h"
class BAlert {
public:
    BAlert(const char* title, const char* text, const char* button1, const char* button2 = nullptr, const char* button3 = nullptr, button_width width = B_WIDTH_AS_USUAL, alert_type type = B_INFO_ALERT) {}
    void Go(void* callback = nullptr) {}
    void SetFlags(uint32 flags) {}
    uint32 Flags() const { return 0; }
};
enum button_width { B_WIDTH_AS_USUAL };
enum alert_type { B_INFO_ALERT, B_IDEA_ALERT, B_WARNING_ALERT, B_STOP_ALERT };
#define B_CLOSE_ON_ESCAPE 0x1
#endif
