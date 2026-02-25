#ifndef _MOCK_OS_H
#define _MOCK_OS_H
#include "SupportDefs.h"
typedef int32 thread_id;
typedef int32 sem_id;
typedef int64 bigtime_t;
#define B_LOW_PRIORITY 5
#define B_LOCAL_TIME 0
inline bigtime_t system_time() { return 0; }
inline sem_id create_sem(int32 count, const char* name) { return 0; }
inline status_t delete_sem(sem_id sem) { return B_OK; }
inline status_t acquire_sem(sem_id sem) { return B_OK; }
inline status_t release_sem(sem_id sem, uint32 count = 1) { return B_OK; }
inline thread_id spawn_thread(status_t (*func)(void*), const char* name, int32 priority, void* data) { return -1; }
inline status_t resume_thread(thread_id thread) { return B_OK; }
inline status_t wait_for_thread(thread_id thread, status_t* exitValue) { return B_OK; }
#endif
