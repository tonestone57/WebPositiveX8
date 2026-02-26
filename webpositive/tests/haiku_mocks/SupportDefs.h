#ifndef _MOCK_SUPPORT_DEFS_H
#define _MOCK_SUPPORT_DEFS_H
#include <stdint.h>
#include <sys/types.h>
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;
typedef int64 bigtime_t;
typedef int32 status_t;
enum {
    B_OK = 0,
    B_ERROR = -1,
    B_BAD_VALUE = -2,
    B_NAME_NOT_FOUND = -3
};
typedef int color_space;
class BBitmap;
enum {
    B_POINTER_EVENTS = 1
};
struct rgb_color {
    uint8_t red, green, blue, alpha;
};
class BVolume;
#ifndef NULL
#define NULL 0
#endif
#define min_c(a,b) ((a)>(b)?(b):(a))
#define max_c(a,b) ((a)>(b)?(a):(b))
#define B_PRIi32 "d"
#define B_USE_SMALL_SPACING 5.0f
#endif
