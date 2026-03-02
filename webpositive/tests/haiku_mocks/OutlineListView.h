#ifndef _OUTLINE_LIST_VIEW_H
#define _OUTLINE_LIST_VIEW_H

#include "ListView.h"
#include "StringItem.h"
#include <vector>
#include <algorithm>
#include <cstdio>

class BOutlineListView : public BListView {
public:
    BOutlineListView(const char* name, list_view_type type = B_SINGLE_SELECTION_LIST) {}
    virtual ~BOutlineListView() {}

    virtual bool AddItem(BListItem* item) {
        fItems.push_back(item);
        return BListView::AddItem(item);
    }
    virtual bool AddItem(BListItem* item, int32 index) {
        if (index >= 0 && index <= (int32)fItems.size())
            fItems.insert(fItems.begin() + index, item);
        else
            fItems.push_back(item);
        return BListView::AddItem(item, index);
    }
    virtual bool AddItem(BListItem* item, BListItem* parent) {
        if (parent == MY_NULLPTR) {
            return AddItem(item);
        } else {
            int32 parentIndex = FullListIndexOf(parent);
            if (parentIndex >= 0) {
                int32 nextIndex = parentIndex + 1;
                while (nextIndex < (int32)fItems.size() && (int32)fItems[nextIndex]->OutlineLevel() > (int32)parent->OutlineLevel()) {
                    nextIndex++;
                }
                return AddItem(item, nextIndex);
            } else {
                return AddItem(item);
            }
        }
    }

    virtual bool RemoveItem(BListItem* item) {
        auto it = std::find(fItems.begin(), fItems.end(), item);
        if (it != fItems.end()) {
            int32 level = (int32)item->OutlineLevel();
            auto next = it + 1;
            while (next != fItems.end() && (int32)(*next)->OutlineLevel() > level) {
                BListItem* subitem = *next;
                next = fItems.erase(next);
                BListView::RemoveItem(subitem);
            }
            fItems.erase(it);
            return BListView::RemoveItem(item);
        }
        return false;
    }
    virtual BListItem* RemoveItem(int32 index) {
        BListItem* item = FullListItemAt(index);
        if (item) RemoveItem(item);
        return item;
    }

    int32 FullListCountItems() const { return (int32)fItems.size(); }
    BListItem* FullListItemAt(int32 index) const {
        if (index >= 0 && index < (int32)fItems.size()) return fItems[index];
        return MY_NULLPTR;
    }
    int32 FullListIndexOf(BListItem* item) const {
        for (size_t i = 0; i < fItems.size(); ++i) {
            if (fItems[i] == item) return (int32)i;
        }
        return -1;
    }

    int32 CountItemsUnder(BListItem* item, bool oneLevelOnly) const {
        int32 index = -1;
        int32 level = -1;
        if (item != MY_NULLPTR) {
            index = FullListIndexOf(item);
            if (index < 0) return 0;
            level = (int32)item->OutlineLevel();
        }

        int32 count = 0;
        for (size_t i = index + 1; i < fItems.size(); ++i) {
            if (level != -1 && (int32)fItems[i]->OutlineLevel() <= level) break;
            if (!oneLevelOnly || (int32)fItems[i]->OutlineLevel() == level + 1)
                count++;
        }
        return count;
    }

    BListItem* ItemUnderAt(BListItem* item, bool oneLevelOnly, int32 index) const {
        int32 parentIndex = -1;
        int32 level = -1;
        if (item != MY_NULLPTR) {
            parentIndex = FullListIndexOf(item);
            if (parentIndex < 0) return MY_NULLPTR;
            level = (int32)item->OutlineLevel();
        }

        int32 current = 0;
        for (size_t i = parentIndex + 1; i < fItems.size(); ++i) {
            if (level != -1 && (int32)fItems[i]->OutlineLevel() <= level) break;
            if (!oneLevelOnly || (int32)fItems[i]->OutlineLevel() == level + 1) {
                if (current == index) return fItems[i];
                current++;
            }
        }
        return MY_NULLPTR;
    }

    virtual void MakeEmpty() {
        fItems.clear();
        BListView::MakeEmpty();
    }

private:
    std::vector<BListItem*> fItems;
};

#endif
