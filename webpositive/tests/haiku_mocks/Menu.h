#ifndef _MENU_H
#define _MENU_H
#include "View.h"
#include "MenuItem.h"
#include <vector>

enum menu_layout {
    B_ITEMS_IN_ROW,
    B_ITEMS_IN_COLUMN,
    B_ITEMS_IN_MATRIX
};

class BMenu : public BView {
public:
    BMenu(const char* title, menu_layout layout = B_ITEMS_IN_COLUMN) : BView(title) {}
    virtual ~BMenu() {
        for (auto item : fItems) delete item;
    }
    void AddItem(BMenuItem* item) { fItems.push_back(item); }
    void AddItem(BMenuItem* item, int32 index) {
        if (index >= 0 && index <= (int32)fItems.size())
            fItems.insert(fItems.begin() + index, item);
        else
            fItems.push_back(item);
    }
    void AddItem(BMenu* menu) { AddItem(new BMenuItem(menu)); }
    void AddItem(BMenu* menu, int32 index) { AddItem(new BMenuItem(menu), index); }

    bool RemoveItem(BMenuItem* item) {
        for (auto it = fItems.begin(); it != fItems.end(); ++it) {
            if (*it == item) { fItems.erase(it); return true; }
        }
        return false;
    }
    bool RemoveItem(BMenu* menu) {
        for (auto it = fItems.begin(); it != fItems.end(); ++it) {
            if ((*it)->Submenu() == menu) {
                BMenuItem* item = *it;
                fItems.erase(it);
                delete item;
                return true;
            }
        }
        return false;
    }
    BMenuItem* RemoveItem(int32 index) {
        if (index >= 0 && index < (int32)fItems.size()) {
            BMenuItem* item = fItems[index];
            fItems.erase(fItems.begin() + index);
            return item;
        }
        return nullptr;
    }
    int32 CountItems() const { return (int32)fItems.size(); }
    BMenuItem* ItemAt(int32 index) const {
        if (index >= 0 && index < (int32)fItems.size()) return fItems[index];
        return nullptr;
    }
    int32 IndexOf(BMenuItem* item) const {
        for (size_t i = 0; i < fItems.size(); ++i) if (fItems[i] == item) return (int32)i;
        return -1;
    }
    int32 IndexOf(BMenu* menu) const {
        for (size_t i = 0; i < fItems.size(); ++i) if (fItems[i]->Submenu() == menu) return (int32)i;
        return -1;
    }

    void SetTargetForItems(BHandler* target) {}
    void SetRadioMode(bool radio) {}

private:
    std::vector<BMenuItem*> fItems;
};
#endif
