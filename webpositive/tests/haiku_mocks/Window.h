#ifndef _WINDOW_H
#define _WINDOW_H
#include "SupportDefs.h"
#include "Message.h"
#include "Rect.h"
#include "Looper.h"

enum window_type {
    B_TITLED_WINDOW,
    B_MODAL_WINDOW,
    B_DOCUMENT_WINDOW
};

enum window_look {
    B_TITLED_WINDOW_LOOK,
    B_BORDERED_WINDOW_LOOK
};

enum window_feel {
    B_NORMAL_WINDOW_FEEL
};

enum {
    B_NOT_ZOOMABLE = 0x0001,
    B_NOT_CLOSABLE = 0x0002,
    B_NOT_MOVABLE = 0x0004,
    B_NOT_RESIZABLE = 0x0008,
    B_AUTO_UPDATE_SIZE_LIMITS = 0x0010,
    B_ASYNCHRONOUS_CONTROLS = 0x0020,
    B_WILL_ACCEPT_FIRST_CLICK = 0x0040,
    B_AVOID_FOCUS = 0x0080
};

enum {
    B_CURRENT_WORKSPACE = 0,
    B_MOVE_IF_PARTIALLY_OFFSCREEN = 1,
    B_DO_NOT_RESIZE_TO_FIT = 2
};

class BView;

class BWindow : public BLooper {
public:
    BWindow(BRect frame, const char* title, window_type type, uint32 flags, uint32 workspaces = B_CURRENT_WORKSPACE) {}
    BWindow(BRect frame, const char* title, window_look look, window_feel feel, uint32 flags, uint32 workspaces = B_CURRENT_WORKSPACE) {}
    virtual ~BWindow() {}
    virtual void MessageReceived(BMessage* message) override {}
    virtual bool QuitRequested() { return true; }
    void Show() {}
    void Hide() {}
    bool IsHidden() const { return false; }
    void CenterOnScreen() {}
    void AddChild(BView* view) {}
    void SetLayout(void* layout) {}
    void Minimize(bool minimize) {}
    bool IsMinimized() const { return false; }
    void MoveOnScreen(uint32 flags) {}
    void SetWorkspaces(uint32 workspaces) {}
    BRect Frame() const { return BRect(0,0,100,100); }
    void MoveTo(float x, float y) {}
    void ResizeTo(float w, float h) {}
};
#endif
