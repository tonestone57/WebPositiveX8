#ifndef _LIST_VIEW_H
#define _LIST_VIEW_H
#include "SupportDefs.h"
#include "Point.h"
#include "Rect.h"
#include "InterfaceDefs.h"

class BMessage;
class BWindow;

class BView {
public:
    BView() {}
    BView(BRect frame, const char* name, uint32 resizingMode, uint32 flags) {}
    virtual ~BView() {}
    virtual void AttachedToWindow() {}
    virtual void MouseDown(BPoint point) {}
    virtual void MessageReceived(BMessage* msg) {}
    void SetEventMask(uint32 mask, uint32 options = 0) {}
    void SetTarget(BView* target) {}
    BWindow* Window() const { return (BWindow*)0; }
    BPoint ConvertToScreen(BPoint point) const { return point; }
    void SetLowColor(rgb_color color) {}
    void SetHighColor(rgb_color color) {}
    void FillRect(BRect rect, drawing_mode mode = B_OP_COPY) {}
    void DrawString(const char* s, BPoint p) {}
    void SetFont(const void* font) {}
    void GetFont(void* font) const {}
    float StringWidth(const char* s) const { return 0; }
    void GetFontHeight(font_height* height) const {
        if (height) { height->ascent = 10; height->descent = 2; height->leading = 1; }
    }
    void SetDrawingMode(drawing_mode mode) {}
    bool LockLooper() { return true; }
    void UnlockLooper() {}
    void AddChild(BView* child) {}
    BRect Bounds() const { return BRect(0,0,100,100); }
    void ResizeTo(float w, float h) {}
    void MoveTo(float x, float y) {}
};

class BListItem {
public:
    BListItem(uint32 outlineLevel = 0, bool expanded = false) {}
    virtual ~BListItem() {}
    virtual void DrawItem(BView* owner, BRect frame, bool complete = false) {}
    bool IsSelected() const { return false; }
};

class BListView : public BView {
public:
    BListView() {}
    BListView(BRect frame, const char* name, uint32 type = 0, uint32 resizingMode = 0, uint32 flags = 0)
        : BView(frame, name, resizingMode, flags) {}
    virtual ~BListView() {}
    virtual void SelectionChanged() {}
    void AddItem(BListItem* item) {}
    void MakeEmpty() {}
    int32 CountItems() const { return 0; }
    BListItem* ItemAt(int32 index) const { return 0; }
    void Select(int32 index) {}
    int32 CurrentSelection(int32 index = 0) const { return -1; }
    void SetInvocationMessage(BMessage* msg) {}
    BRect ItemFrame(int32 index) const { return BRect(); }
    void ScrollTo(float x, float y) {}
    void DeselectAll() {}
    void ScrollToSelection() {}
};

#define B_FOLLOW_NONE 0
#define B_NO_BORDER 0

#endif
