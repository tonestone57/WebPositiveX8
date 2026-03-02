#ifndef _LOOPER_H
#define _LOOPER_H
#include "Handler.h"
#include "OS.h"
#include <vector>

class BMessage;

class BLooper : public BHandler {
public:
    BLooper(const char* name = MY_NULLPTR) : BHandler(name) {
        SetLooper(this);
    }
    virtual ~BLooper() {}
    virtual void MessageReceived(BMessage* message) override {}
    status_t PostMessage(uint32 what) {
        BMessage msg(what);
        return PostMessage(&msg);
    }
    status_t PostMessage(BMessage* message) {
        if (!message) return B_BAD_VALUE;
        for (auto h : fHandlers) {
            h->MessageReceived(message);
        }
        this->MessageReceived(message);
        return B_OK;
    }
    void AddHandler(BHandler* handler) {
        if (handler) {
            fHandlers.push_back(handler);
            handler->SetLooper(this);
        }
    }
    thread_id Run() { return 1; }
    bool Lock() { return true; }
    void Unlock() {}
    void Quit() {}
    static BLooper* LooperForThread(thread_id thread) { return MY_NULLPTR; }

private:
    std::vector<BHandler*> fHandlers;
};
#endif
