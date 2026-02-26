#ifndef _HANDLER_H
#define _HANDLER_H
#include "SupportDefs.h"
#include "Message.h"
class BLooper;
class BHandler {
public:
    BHandler(const char* name = nullptr) {}
    virtual ~BHandler() {}
    virtual void MessageReceived(BMessage* message) {}
    BLooper* Looper() const { return nullptr; }
};
#endif
