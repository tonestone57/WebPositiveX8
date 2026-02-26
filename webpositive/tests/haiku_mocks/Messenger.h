#ifndef _MESSENGER_H
#define _MESSENGER_H
#include "SupportDefs.h"
class BHandler;
class BMessenger {
public:
    BMessenger() {}
    BMessenger(BHandler* handler) {}
    status_t SendMessage(uint32 what) const { return B_OK; }
};
#endif
