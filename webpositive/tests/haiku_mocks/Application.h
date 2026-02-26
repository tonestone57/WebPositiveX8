#ifndef _APPLICATION_H
#define _APPLICATION_H
#include "Looper.h"
class BApplication : public BLooper {
public:
    BApplication(const char* signature);
    virtual ~BApplication() {}
    virtual void ReadyToRun() {}
};
extern BApplication* be_app;
#endif
