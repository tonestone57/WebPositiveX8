#ifndef _NODE_INFO_H
#define _NODE_INFO_H
#include "SupportDefs.h"

enum icon_size { B_MINI_ICON = 16, B_LARGE_ICON = 32 };

class BBitmap;
class BNodeInfo {
public:
    BNodeInfo(BNode* node) {}
    status_t GetTrackerIcon(BBitmap* icon, icon_size size) { return B_OK; }
    status_t GetType(char* type) { return B_OK; }
};
#endif
