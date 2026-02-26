#ifndef _NODE_MONITOR_H
#define _NODE_MONITOR_H
#include "SupportDefs.h"
#include "Messenger.h"

enum {
    B_WATCH_DIRECTORY = 0x01,
    B_WATCH_ALL = 0xFF
};
enum {
    B_ENTRY_CREATED = 1,
    B_ENTRY_REMOVED = 2,
    B_ENTRY_MOVED = 3,
    B_ATTR_CHANGED = 4
};

inline status_t watch_node(const node_ref* node, uint32 flags, BMessenger target) { return B_OK; }
inline status_t watch_node(const node_ref* node, uint32 flags, BHandler* target) { return B_OK; }
inline status_t stop_watching(BMessenger target) { return B_OK; }
inline status_t stop_watching(BHandler* target) { return B_OK; }
#endif
