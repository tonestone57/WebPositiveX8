#ifndef _LOOPER_H
#define _LOOPER_H
#include "Handler.h"
#include "OS.h"
class BMessage;
class BLooper : public BHandler {
public:
    BLooper(const char* name = nullptr) : BHandler(name) {}
    virtual ~BLooper() {}
    virtual void MessageReceived(BMessage* message) override {}
    status_t PostMessage(uint32 what) { return B_OK; }
    status_t PostMessage(BMessage* message) { return B_OK; }
    void AddHandler(BHandler* handler) {}
    thread_id Run() { return 1; }
    bool Lock() { return true; }
    void Unlock() {}
    void Quit() {}
    static BLooper* LooperForThread(thread_id thread) { return nullptr; }
};
#endif
