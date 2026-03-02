#ifndef BEOS_COMPATIBILITY_H
#define BEOS_COMPATIBILITY_H

#if defined(__cplusplus) && __cplusplus >= 201103L
#define MY_NULLPTR nullptr
#else
#define MY_NULLPTR NULL
#endif

#endif // BEOS_COMPATIBILITY_H
