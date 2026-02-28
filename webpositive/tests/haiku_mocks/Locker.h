#ifndef _MOCK_LOCKER_H
#define _MOCK_LOCKER_H
#include "SupportDefs.h"
#include "OS.h"
class BLocker {
public:
    BLocker(const char* name = MY_NULLPTR) {}
    bool Lock() { return true; }
    void Unlock() {}
};
#endif
