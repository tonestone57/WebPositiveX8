#ifndef _WINDOW_H
#define _WINDOW_H
#include "Looper.h"
#include "View.h"

enum window_look {
    B_TITLED_WINDOW_LOOK,
    B_TITLED_WINDOW,
    B_BORDERED_WINDOW_LOOK,
    B_DOCUMENT_WINDOW_LOOK
};
enum window_feel {
    B_NORMAL_WINDOW_FEEL,
    B_MODAL_APP_WINDOW_FEEL
};
typedef enum {
    B_UNTYPED_WINDOW,
    B_TITLED_WINDOW_TYPE,
    B_DOCUMENT_WINDOW_TYPE
} window_type;

enum {
    B_AUTO_UPDATE_SIZE_LIMITS = 0,
    B_ASYNCHRONOUS_CONTROLS = 0,
    B_NOT_ZOOMABLE = 0x0001,
    B_NOT_MOVABLE = 0x0002,
    B_WILL_ACCEPT_FIRST_CLICK = 0x0004,
    B_AVOID_FOCUS = 0x0008
};
#define B_CURRENT_WORKSPACE 0

class BWindow : public BLooper {
public:
    BWindow(BRect frame, const char* title, window_look look, window_feel feel, uint32 flags, uint32 workspaces = B_CURRENT_WORKSPACE) : fFrame(frame) {}
    BWindow(BRect frame, const char* title, window_type type, uint32 flags, uint32 workspaces = B_CURRENT_WORKSPACE) : fFrame(frame) {}
    BWindow(BRect frame, const char* title, window_look look, int feel, uint32 flags, uint32 workspaces = B_CURRENT_WORKSPACE) : fFrame(frame) {}
    BWindow(BRect frame, const char* title, int look, int flags) : fFrame(frame) {}

    virtual ~BWindow() {}
    void Show() {}
    void Hide() {}
    bool IsHidden() const { return false; }
    void AddChild(BView* view);
    void CenterOnScreen() {}
    BMessage* CurrentMessage() const { return nullptr; }
    BRect Frame() const { return fFrame; }
    void MoveTo(float x, float y) { fFrame.OffsetTo(x, y); }
    void ResizeTo(float w, float h) { fFrame.right = fFrame.left + w; fFrame.bottom = fFrame.top + h; }
    void SetLayout(BLayout* layout) {}
    void Activate(bool active = true) {}
    void SetWorkspaces(uint32 workspaces) {}
    uint32 Workspaces() const { return 0; }
    void GetDecoratorSettings(BMessage* settings) const {}
    BRect Bounds() const { return BRect(0, 0, fFrame.Width(), fFrame.Height()); }
    virtual bool QuitRequested() { return true; }
private:
    BRect fFrame;
};
#endif
