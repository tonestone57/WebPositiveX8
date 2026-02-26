#ifndef _VIEW_H
#define _VIEW_H

#include "SupportDefs.h"
#include "Rect.h"
#include "Point.h"
#include "Size.h"
#include "Message.h"
#include "InterfaceDefs.h"
#include "Font.h"
#include "Handler.h"
#include <vector>

enum border_style {
    B_NO_BORDER,
    B_PLAIN_BORDER,
    B_FANCY_BORDER
};

enum {
    B_WILL_DRAW = 0x0001,
    B_PULSE_NEEDED = 0x0002,
    B_FRAME_EVENTS = 0x0004,
    B_POINTER_EVENTS = 0x0008,
    B_FULL_UPDATE_ON_RESIZE = 0x0010
};

#define B_FOLLOW_NONE 0
#define B_PRIMARY_MOUSE_BUTTON 1
#define B_SECONDARY_MOUSE_BUTTON 2
#define B_TERTIARY_MOUSE_BUTTON 4
#define B_LOCK_WINDOW_FOCUS 0x0100

class BWindow;
class BLayout;
class BLayoutItem;
class BBitmap;
class BToolTip {};

class BView : public BHandler {
public:
    BView() : BHandler(nullptr), fParent(nullptr), fWindow(nullptr), fFrame(0, 0, -1, -1), fLayout(nullptr) {}
    BView(const char* name, uint32 flags = 0) : BHandler(name), fParent(nullptr), fWindow(nullptr), fFrame(0, 0, -1, -1), fLayout(nullptr) {}
    BView(BRect frame, const char* name, uint32 resizingMode, uint32 flags) : BHandler(name), fParent(nullptr), fWindow(nullptr), fFrame(frame), fLayout(nullptr) {}
    virtual ~BView(); // Defined in Mocks.cpp or here if no other dependency

    virtual void AttachedToWindow() {}
    virtual void AllAttached() {}
    virtual void DetachedFromWindow() {}
    virtual void AllDetached() {}
    virtual void Draw(BRect updateRect) {}
    virtual void MouseDown(BPoint where) {}
    virtual void MouseUp(BPoint where) {}
    virtual void MouseMoved(BPoint where, uint32 transit, const BMessage* dragMessage) {}
    virtual void FrameResized(float newWidth, float newHeight) {
        fFrame.right = fFrame.left + newWidth;
        fFrame.bottom = fFrame.top + newHeight;
    }
    virtual void MessageReceived(BMessage* message) override {}
    virtual void KeyDown(const char* bytes, int32 numBytes) {}
    virtual void Pulse() {}

    void AddChild(BView* child);
    bool RemoveChild(BView* child);
    BView* ChildAt(int32 index) const {
        if (index >= 0 && index < (int32)fChildren.size()) return fChildren[index];
        return nullptr;
    }
    int32 CountChildren() const { return (int32)fChildren.size(); }
    BView* Parent() const { return fParent; }
    BWindow* Window() const { return fWindow; }

    BRect Bounds() const { return BRect(0, 0, fFrame.Width(), fFrame.Height()); }
    BRect Frame() const { return fFrame; }
    BPoint ConvertToScreen(BPoint point) const { return point; }
    BRect ConvertToScreen(BRect rect) const { return rect; }
    void ConvertToScreen(BPoint* point) const {}
    void ConvertToScreen(BRect* rect) const {}
    void ConvertFromScreen(BPoint* point) const {}
    void Invalidate() {}
    void Invalidate(BRect rect) {}
    void MakeFocus(bool focus = true) {}
    bool IsFocus() const { return false; }
    bool IsHidden() const { return false; }
    void Show() {}
    void Hide() {}
    void SetFlags(uint32 flags) {}
    uint32 Flags() const { return 0; }
    void SetViewUIColor(int32 color) {}
    void SetViewColor(rgb_color color) {}
    void SetViewColor(int32 color) {}
    void SetViewColor(uint32 color) {}
    void SetLowUIColor(int32 color) {}
    void SetHighColor(rgb_color color) {}
    void SetLowColor(rgb_color color) {}
    void SetPenSize(float size) {}
    void FillRect(BRect rect, drawing_mode mode = B_OP_COPY) {}
    void FillRoundRect(BRect rect, float xRadius, float yRadius, drawing_mode mode = B_OP_COPY) {}
    void StrokeLine(BPoint start, BPoint end) {}
    float StringWidth(const char* string) const { return 100.0f; }

    void SetExplicitMinSize(BSize size) {}
    void SetExplicitMaxSize(BSize size) {}
    virtual BSize MinSize();
    virtual BSize MaxSize() { return BSize(10000, 10000); }
    virtual BSize PreferredSize() { return BSize(100, 100); }

    void SetEventMask(uint32 mask, uint32 options = 0) {}
    void SetMouseEventMask(uint32 mask, uint32 options = 0) {}
    void GetMouse(BPoint* where, uint32* buttons, bool checkMessageQueue = true) {
        if (where) *where = BPoint(0, 0);
        if (buttons) *buttons = 0;
    }
    void SetTarget(BView* target) {}
    void SetTarget(BHandler* target) {}
    void SetFont(const BFont* font) {}
    void GetFont(BFont* font) const {}
    void GetFontHeight(font_height* height) const {
        if (height) { height->ascent = 10; height->descent = 2; height->leading = 1; }
    }
    void SetDrawingMode(drawing_mode mode) {}
    bool LockLooper() { return true; }
    void UnlockLooper() {}
    void ResizeTo(float w, float h) {
        fFrame.right = fFrame.left + w;
        fFrame.bottom = fFrame.top + h;
    }
    void MoveTo(float x, float y) {
        float w = fFrame.Width();
        float h = fFrame.Height();
        fFrame.left = x;
        fFrame.top = y;
        fFrame.right = x + w;
        fFrame.bottom = y + h;
    }
    void InvalidateItem(int32 index) {}
    void DrawString(const char* s, BPoint p) {}
    void DrawString(const char* s, BView* owner, BRect r) {}
    void SetLayout(BLayout* layout);
    BLayoutItem* LayoutItem() const { return nullptr; }
    void HideToolTip() {}
    void SetToolTip(const char* text) {}
    void SetToolTip(void* toolTip) {}
    void SetBlendingMode(int32 src, int32 dst) {}
    void DrawBitmap(const BBitmap* bitmap, BRect src, BRect dst, uint32 flags = 0) {}
    BLayout* GetLayout() const { return fLayout; }
    bool IsEnabled() const { return true; }

private:
    BView* fParent;
    BWindow* fWindow;
    std::vector<BView*> fChildren;
    BRect fFrame;
    BLayout* fLayout;
};

#endif
