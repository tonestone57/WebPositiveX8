#ifndef _MOCK_FILE_H
#define _MOCK_FILE_H
#include "SupportDefs.h"
class BFile {
public:
    BFile() {}
    BFile(const char* path, uint32 mode) {}
    status_t SetTo(const char* path, uint32 mode) { return B_OK; }
    status_t SetPermissions(uint32 mode) { return B_OK; }
};
enum {
    B_READ_ONLY = 0,
    B_WRITE_ONLY = 1,
    B_READ_WRITE = 2,
    B_CREATE_FILE = 4,
    B_ERASE_FILE = 8
};
#endif
