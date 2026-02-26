#ifndef _MOCK_FIND_DIRECTORY_H
#define _MOCK_FIND_DIRECTORY_H
#include "SupportDefs.h"
#include "Path.h"
#include <stdio.h>

enum directory_which {
    B_USER_SETTINGS_DIRECTORY
};

typedef status_t (*find_directory_func)(directory_which, BPath*, bool, const BVolume*);
extern find_directory_func gMockFindDirectory;

inline status_t find_directory(directory_which which, BPath* path, bool create_it = false, const BVolume* volume = NULL) {
    if (gMockFindDirectory)
        return gMockFindDirectory(which, path, create_it, volume);
    return B_ERROR;
}
#endif
