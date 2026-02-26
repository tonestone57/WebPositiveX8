#ifndef _LOOPER_H
#define _LOOPER_H
#include "SupportDefs.h"
#include "Message.h"
class BLooper {
public:
    virtual ~BLooper() {}
    virtual void MessageReceived(BMessage* message) {}
    status_t PostMessage(uint32 what) { return B_OK; }
    status_t PostMessage(BMessage* message) { return B_OK; }
    void Run() {}
    void Quit() {}
    bool Lock() { return true; }
    void Unlock() {}
};
#endif
