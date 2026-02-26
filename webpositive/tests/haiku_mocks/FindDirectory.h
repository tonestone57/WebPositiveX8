#ifndef _FIND_DIRECTORY_H
#define _FIND_DIRECTORY_H
#include "SupportDefs.h"
#include "Path.h"

typedef enum {
    B_USER_SETTINGS_DIRECTORY,
    B_TRASH_DIRECTORY
} directory_which;

class BVolume;
typedef status_t (*find_directory_func)(directory_which, BPath*, bool, const BVolume*);
extern find_directory_func gMockFindDirectory;

inline status_t find_directory(directory_which which, BPath* path, bool create_it = false, const BVolume* volume = NULL) {
    if (gMockFindDirectory) return gMockFindDirectory(which, path, create_it, volume);
    if (path) path->SetTo("/tmp/settings");
    return B_OK;
}
#endif
