#ifndef _MESSENGER_H
#define _MESSENGER_H
#include "SupportDefs.h"
#include "Looper.h"
#include "Handler.h"

class BMessenger {
public:
    BMessenger() : fHandler(nullptr) {}
    BMessenger(const char* signature, team_id team = -1, status_t* error = nullptr) : fHandler(nullptr) {}
    BMessenger(BHandler* handler, BLooper* looper = nullptr, status_t* error = nullptr) : fHandler(handler) {}
    status_t SendMessage(uint32 what) const {
        BMessage msg(what);
        return SendMessage(&msg);
    }
    status_t SendMessage(const BMessage* message) const {
        if (fHandler) {
            fHandler->MessageReceived(const_cast<BMessage*>(message));
        }
        return B_OK;
    }
    bool IsValid() const { return true; }
private:
    BHandler* fHandler;
};
extern BMessenger be_app_messenger;
#endif
