#ifndef _SUPPORT_DEFS_H
#define _SUPPORT_DEFS_H

#ifndef MY_NULLPTR
#if defined(__cplusplus) && __cplusplus >= 201103L
#define MY_NULLPTR nullptr
#else
#define MY_NULLPTR NULL
#endif
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int8_t int8;
typedef uint8_t uint8;

typedef int32 status_t;
typedef int32 team_id;
typedef int32 thread_id;
typedef int32 sem_id;
typedef int64 bigtime_t;

enum {
    B_OK = 0,
    B_ERROR = -1,
    B_BAD_VALUE = -2,
    B_NAME_NOT_FOUND = -3,
    B_NO_MEMORY = -4,
    B_ENTRY_NOT_FOUND = -5
};

typedef struct {
    uint8 red;
    uint8 green;
    uint8 blue;
    uint8 alpha;
    bool IsLight() const { return (red + green + blue) / 3 > 128; }
} rgb_color;

#define B_PRIi32 "d"
#define B_PRIu32 "u"

struct entry_ref {
    dev_t device;
    ino_t directory;
    char* name;
    entry_ref() : device(0), directory(0), name(MY_NULLPTR) {}
    entry_ref(dev_t d, ino_t dir, const char* n) : device(d), directory(dir), name(n ? strdup(n) : MY_NULLPTR) {}
    ~entry_ref() { free(name); }
    entry_ref(const entry_ref& other) : device(other.device), directory(other.directory), name(other.name ? strdup(other.name) : MY_NULLPTR) {}
    entry_ref& operator=(const entry_ref& other) {
        if (this != &other) {
            device = other.device; directory = other.directory;
            free(name); name = other.name ? strdup(other.name) : MY_NULLPTR;
        }
        return *this;
    }
    void set_name(const char* n) { free(name); name = n ? strdup(n) : MY_NULLPTR; }
};

typedef struct {
    dev_t device;
    ino_t node;
} node_ref;

#define min_c(a,b) ((a)<(b)?(a):(b))
#define max_c(a,b) ((a)>(b)?(a):(b))

#define B_REFS_RECEIVED 'refs'
#define B_RAW_TYPE 'rawt'
#define B_FILE_NAME_LENGTH 256
#define B_NODE_MONITOR 'nmtr'

typedef struct { int32 id; } screen_id;

#endif
