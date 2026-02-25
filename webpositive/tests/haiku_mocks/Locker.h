#ifndef _MOCK_LOCKER_H
#define _MOCK_LOCKER_H
#include "SupportDefs.h"
class BLocker {
public:
    BLocker(const char* name = NULL) {}
    bool Lock() { return true; }
    void Unlock() {}
};
#endif
