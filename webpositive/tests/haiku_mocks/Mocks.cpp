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

BWindow::~BWindow() {
    auto topViews = fTopViews;
    fTopViews.clear();
    for (auto view : topViews) {
        view->SetParent(nullptr);
        view->_SetWindow(nullptr);
        delete view;
    }
}

void BWindow::AddChild(BView* view) {
    if (view) {
        if (view->Parent()) view->Parent()->RemoveChild(view);
        view->SetParent((BView*)this); // Type punning for mock
        view->SetWindow(this);
        view->_SetWindow(this);
        fTopViews.push_back(view);
        view->AttachedToWindow();
    }
}

void BWindow::RemoveChild(BView* view) {
    for (auto it = fTopViews.begin(); it != fTopViews.end(); ++it) {
        if (*it == view) {
            fTopViews.erase(it);
            view->DetachedFromWindow();
            view->SetParent(nullptr);
            view->SetWindow(nullptr);
            view->_SetWindow(nullptr);
            return;
        }
    }
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

BView::~BView() {
    if (fParent) fParent->RemoveChild(this);
    delete fLayout; fLayout = nullptr;
    auto children = fChildren;
    fChildren.clear();
    for (auto child : children) {
        child->fParent = nullptr; // Prevent child from trying to remove itself from us while we iterate
        delete child;
    }
}

void BView::_SetWindow(BWindow* window) {
    fWindow = window;
    for (auto child : fChildren) child->_SetWindow(window);
}

void BView::AddChild(BView* child) {
    if (!child) return;
    if (child->fParent) child->fParent->RemoveChild(child);
    fChildren.push_back(child);
    child->fParent = this;
    child->_SetWindow(fWindow);
    if (fWindow) child->AttachedToWindow();
}

bool BView::RemoveChild(BView* child) {
    for (std::vector<BView*>::iterator it = fChildren.begin(); it != fChildren.end(); ++it) {
        if (*it == child) {
            fChildren.erase(it);
            if (fWindow) child->DetachedFromWindow();
            child->fParent = nullptr;
            child->_SetWindow(nullptr);
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
