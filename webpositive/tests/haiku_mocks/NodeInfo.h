#ifndef _NODE_INFO_H
#define _NODE_INFO_H
#include "Node.h"
#include "Bitmap.h"
class BNodeInfo {
public:
    BNodeInfo(BNode* node) {}
    status_t GetTrackerIcon(BBitmap* icon, icon_size size) { return B_OK; }
};
enum icon_size { B_MINI_ICON, B_LARGE_ICON };
#endif
