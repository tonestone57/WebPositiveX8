#ifndef _ALERT_H
#define _ALERT_H
#include "SupportDefs.h"

enum button_width { B_WIDTH_AS_USUAL };
enum alert_type { B_INFO_ALERT, B_WARNING_ALERT };

class BAlert {
public:
    BAlert(const char* title, const char* text, const char* button1, const char* button2 = MY_NULLPTR, const char* button3 = MY_NULLPTR, button_width width = B_WIDTH_AS_USUAL, alert_type type = B_INFO_ALERT) {}
    void SetShortcut(int32 index, uint32 key) {}
    int32 Go(void* data = MY_NULLPTR) { return 0; }
    uint32 Flags() const { return 0; }
    void SetFlags(uint32 flags) {}
};
#define B_CLOSE_ON_ESCAPE 0x01
#endif
