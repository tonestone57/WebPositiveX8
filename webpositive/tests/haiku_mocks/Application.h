#ifndef _MOCK_APPLICATION_H
#define _MOCK_APPLICATION_H
#include "Looper.h"
#include "OS.h"
class BWindow;
class BApplication : public BLooper {
public:
    BApplication(const char* signature) {}
};
#endif
