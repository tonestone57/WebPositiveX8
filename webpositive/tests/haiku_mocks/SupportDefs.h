#ifndef _MOCK_SUPPORT_DEFS_H
#define _MOCK_SUPPORT_DEFS_H
#include <stdint.h>
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;
typedef int32 status_t;
enum {
    B_OK = 0,
    B_ERROR = -1
};
enum {
    B_POINTER_EVENTS = 1
};
struct rgb_color {
    uint8_t red, green, blue, alpha;
};
#ifndef NULL
#define NULL 0
#endif
#define min_c(a,b) ((a)>(b)?(b):(a))
#define max_c(a,b) ((a)>(b)?(a):(b))
#endif
