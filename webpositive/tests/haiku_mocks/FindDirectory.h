#ifndef _MOCK_FIND_DIRECTORY_H
#define _MOCK_FIND_DIRECTORY_H
#include "SupportDefs.h"
#include "Path.h"
enum directory_which {
    B_USER_SETTINGS_DIRECTORY
};
inline status_t find_directory(directory_which which, BPath* path) { return B_ERROR; }
#endif
