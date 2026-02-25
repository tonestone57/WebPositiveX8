#ifndef _MOCK_PATH_H
#define _MOCK_PATH_H
#include "SupportDefs.h"
class BPath {
public:
    BPath() {}
    BPath(const char* path) {}
    const char* Path() const { return ""; }
};
#endif
