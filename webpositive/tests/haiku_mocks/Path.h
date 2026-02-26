#ifndef _MOCK_PATH_H
#define _MOCK_PATH_H
#include "SupportDefs.h"
#include <string>
class BPath {
public:
    BPath() : fPath("") {}
    BPath(const char* path) : fPath(path ? path : "") {}
    status_t SetTo(const char* path) { fPath = path ? path : ""; return B_OK; }
    status_t Append(const char* path) {
        if (!fPath.empty() && fPath.back() != '/') fPath += '/';
        fPath += path;
        return B_OK;
    }
    const char* Path() const { return fPath.c_str(); }
private:
    std::string fPath;
};
#endif
