#ifndef _MESSENGER_H
#define _MESSENGER_H
#include "SupportDefs.h"
#include "Looper.h"
class BMessenger {
public:
    BMessenger() {}
    BMessenger(const char* signature, team_id team = -1, status_t* error = nullptr) {}
    BMessenger(BHandler* handler, BLooper* looper = nullptr, status_t* error = nullptr) {}
    status_t SendMessage(uint32 what) const { return B_OK; }
    status_t SendMessage(const BMessage* message) const { return B_OK; }
    bool IsValid() const { return true; }
};
extern BMessenger be_app_messenger;
#endif
