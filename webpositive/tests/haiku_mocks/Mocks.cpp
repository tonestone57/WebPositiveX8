#include "FindDirectory.h"
#include "Clipboard.h"
#include "ControlLook.h"
#include "Application.h"
#include "Window.h"
#include "Entry.h"
#include "Path.h"
#include "Messenger.h"
#include "OS.h"
#include "Layout.h"
#include <cstdio>

find_directory_func gMockFindDirectory = MY_NULLPTR;

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
BApplication* be_app = MY_NULLPTR;

void BWindow::AddChild(BView* view) {}

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

BView::~BView() {
    if (fParent) fParent->RemoveChild(this);
    delete fLayout; fLayout = MY_NULLPTR;
    auto children = fChildren;
    fChildren.clear();
    for (auto child : children) {
        child->fParent = MY_NULLPTR; // Prevent child from trying to remove itself from us while we iterate
        delete child;
    }
}

void BView::AddChild(BView* child) {
    if (!child) return;
    if (child->fParent) child->fParent->RemoveChild(child);
    fChildren.push_back(child);
    child->fParent = this;
}

bool BView::RemoveChild(BView* child) {
    for (std::vector<BView*>::iterator it = fChildren.begin(); it != fChildren.end(); ++it) {
        if (*it == child) {
            fChildren.erase(it);
            child->fParent = MY_NULLPTR;
            return true;
        }
    }
    return false;
}

void BView::SetLayout(BLayout* layout) {
    if (fLayout) delete fLayout;
    fLayout = layout;
    if (fLayout) fLayout->SetView(this);
}

BSize BView::MinSize() {
    if (fLayout) return fLayout->MinSize();
    return BSize(0, 0);
}
