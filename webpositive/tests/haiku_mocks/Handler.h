#ifndef _HANDLER_H
#define _HANDLER_H
#include "SupportDefs.h"
#include "Message.h"
class BLooper;
class BHandler {
public:
    BHandler(const char* name = nullptr) : fName(name ? name : ""), fLooper(nullptr) {}
    virtual ~BHandler() {}
    virtual void MessageReceived(BMessage* message) {}
    BLooper* Looper() const { return fLooper; }
    void SetLooper(BLooper* looper) { fLooper = looper; }
    const char* Name() const { return fName.String(); }
    void SetName(const char* name) { fName = name ? name : ""; }
private:
    BString fName;
    BLooper* fLooper;
};
#endif
