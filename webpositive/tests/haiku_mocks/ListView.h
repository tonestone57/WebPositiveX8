#ifndef _LIST_VIEW_H
#define _LIST_VIEW_H

#include "View.h"

enum list_view_type {
    B_SINGLE_SELECTION_LIST,
    B_MULTIPLE_SELECTION_LIST
};

class BListItem {
public:
    BListItem(uint32 outlineLevel = 0, bool expanded = false)
        : fOutlineLevel(outlineLevel), fExpanded(expanded) {}
    virtual ~BListItem() {}
    virtual void DrawItem(BView* owner, BRect frame, bool complete = false) {}
    bool IsSelected() const { return false; }
    uint32 OutlineLevel() const { return fOutlineLevel; }
    void SetOutlineLevel(uint32 level) { fOutlineLevel = level; }
    bool IsExpanded() const { return fExpanded; }
    void SetExpanded(bool expanded) { fExpanded = expanded; }
    void SetEnabled(bool enabled) {}
private:
    uint32 fOutlineLevel;
    bool fExpanded;
};

class BListView : public BView {
public:
    BListView() {}
    BListView(const char* name, list_view_type type = B_SINGLE_SELECTION_LIST) {}
    BListView(BRect frame, const char* name, list_view_type type = B_SINGLE_SELECTION_LIST, uint32 resizingMode = 0, uint32 flags = 0)
        : BView(frame, name, resizingMode, flags) {}
    virtual ~BListView() { MakeEmpty(); }
    virtual void SelectionChanged() {}
    virtual bool AddItem(BListItem* item) { fItems.push_back(item); return true; }
    virtual bool AddItem(BListItem* item, int32 index) {
        if (index >= 0 && index <= (int32)fItems.size())
            fItems.insert(fItems.begin() + index, item);
        else
            fItems.push_back(item);
        return true;
    }
    virtual void MakeEmpty() {
        // NOTE: Standard BListView does NOT delete items.
        // But for our mocks it might be easier.
        // Actually, CookieWindow.cpp deletes them manually.
        // So we should NOT delete them here if we want to be realistic.
        fItems.clear();
    }
    int32 CountItems() const { return (int32)fItems.size(); }
    BListItem* ItemAt(int32 index) const {
        if (index < 0 || index >= (int32)fItems.size()) return nullptr;
        return fItems[index];
    }
    virtual BListItem* RemoveItem(int32 index) {
        if (index < 0 || index >= (int32)fItems.size()) return nullptr;
        BListItem* item = fItems[index];
        fItems.erase(fItems.begin() + index);
        return item;
    }
    virtual bool RemoveItem(BListItem* item) {
        for (auto it = fItems.begin(); it != fItems.end(); ++it) {
            if (*it == item) { fItems.erase(it); return true; }
        }
        return false;
    }
    void Select(int32 index) {}
    int32 CurrentSelection(int32 index = 0) const { return -1; }
    void SetInvocationMessage(BMessage* msg) {}
    BRect ItemFrame(int32 index) const { return BRect(); }
    void ScrollTo(float x, float y) {}
    void DeselectAll() {}
    void ScrollToSelection() {}
    void SetSelectionMessage(BMessage* msg) {}

private:
    std::vector<BListItem*> fItems;
};

#endif
