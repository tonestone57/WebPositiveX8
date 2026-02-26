#ifndef _OS_H
#define _OS_H
#include "SupportDefs.h"

#define B_LOW_PRIORITY 5
#define B_DISPLAY_PRIORITY 10
#define B_URGENT_DISPLAY_PRIORITY 15

#ifdef __cplusplus
extern "C" {
#endif

sem_id create_sem(int32 count, const char* name);
status_t delete_sem(sem_id sem);
status_t acquire_sem(sem_id sem);
status_t release_sem(sem_id sem);

typedef status_t (*thread_func)(void*);
thread_id spawn_thread(thread_func func, const char* name, int32 priority, void* data);
status_t resume_thread(thread_id thread);
status_t wait_for_thread(thread_id thread, status_t* exit_value);

bigtime_t system_time();
void snooze(bigtime_t microseconds);
void get_click_speed(bigtime_t* speed);

#ifdef __cplusplus
}
#endif

#endif
