#ifndef _MOCK_ENTRY_H
#define _MOCK_ENTRY_H
#include "SupportDefs.h"
class BEntry {
public:
    BEntry() {}
    BEntry(const char* path) {}
    bool Exists() const { return false; }
};
#endif
