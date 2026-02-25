#ifndef _MESSAGE_H
#define _MESSAGE_H
#include "SupportDefs.h"
class BMessage {
public:
    BMessage(uint32 what) : what(what) {}
    uint32 what;
};
#endif
