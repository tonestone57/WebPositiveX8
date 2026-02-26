#ifndef _MESSAGE_RUNNER_H
#define _MESSAGE_RUNNER_H
#include "Messenger.h"
class BMessageRunner {
public:
    BMessageRunner(BMessenger target, const BMessage* message, bigtime_t interval, int32 count = -1) {}
};
#endif
