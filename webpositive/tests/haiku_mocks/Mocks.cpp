#include "FindDirectory.h"
#include "Clipboard.h"
#include "ControlLook.h"
#include "Application.h"
#include "Window.h"
#include "Entry.h"
#include "Path.h"
#include "Messenger.h"
#include "OS.h"
#include <cstdio>

find_directory_func gMockFindDirectory = nullptr;

BClipboard::BClipboard(const char* name, bool lookForExternalChanges) : fData(0) { printf("BClipboard constructor\n"); }
BClipboard sClipboard("system");
BClipboard* be_clipboard = &sClipboard;

BControlLook::BControlLook() { printf("BControlLook constructor\n"); }
BControlLook sControlLook;
BControlLook* be_control_look = &sControlLook;

BApplication::BApplication(const char* signature) : BLooper(signature) {
    printf("BApplication constructor\n");
    be_app = this;
}
BApplication* be_app = nullptr;

void BWindow::AddChild(BView* view) {}

status_t BEntry::GetPath(BPath* path) const {
    if (path) path->SetTo("/tmp/test");
    return B_OK;
}

BMessenger be_app_messenger;

void snooze(bigtime_t microseconds) {}
bigtime_t system_time() { return 1000; }
void get_click_speed(bigtime_t* speed) { if (speed) *speed = 500000; }

sem_id create_sem(int32 count, const char* name) { return 1; }
status_t delete_sem(sem_id sem) { return B_OK; }
status_t acquire_sem(sem_id sem) { return B_OK; }
status_t release_sem(sem_id sem) { return B_OK; }

thread_id spawn_thread(thread_func func, const char* name, int32 priority, void* data) { return 1; }
status_t resume_thread(thread_id thread) { return B_OK; }
status_t wait_for_thread(thread_id thread, status_t* exit_value) { if (exit_value) *exit_value = B_OK; return B_OK; }
