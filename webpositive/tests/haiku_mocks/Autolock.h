#ifndef _MOCK_AUTOLOCK_H
#define _MOCK_AUTOLOCK_H
#include "Locker.h"
class BAutolock {
public:
    BAutolock(BLocker* locker) : fLocker(locker) { if (fLocker) fLocker->Lock(); }
    BAutolock(BLocker& locker) : fLocker(&locker) { fLocker->Lock(); }
    ~BAutolock() { if (fLocker) fLocker->Unlock(); }
    bool IsLocked() const { return fLocker != nullptr; }
private:
    BLocker* fLocker;
};
#endif
