#ifndef _MOCK_OBJECT_LIST_H
#define _MOCK_OBJECT_LIST_H
#include "SupportDefs.h"
#include <vector>
#include <algorithm>

template<class T, bool Owns = true>
class BObjectList {
public:
    BObjectList(int32 itemsPerBlock = 20, bool owns = Owns) : fOwns(owns) {}
    ~BObjectList() {
        if (fOwns) {
            for (T* item : fItems) delete item;
        }
    }
    bool AddItem(T* item) { fItems.push_back(item); return true; }
    T* RemoveItem(int32 index) {
        T* item = fItems[index];
        fItems.erase(fItems.begin() + index);
        return item;
    }
    T* ItemAt(int32 index) const {
        if (index < 0 || index >= (int32)fItems.size()) return MY_NULLPTR;
        return fItems[index];
    }
    int32 CountItems() const { return (int32)fItems.size(); }
    void MakeEmpty() {
        if (fOwns) {
            for (T* item : fItems) delete item;
        }
        fItems.clear();
    }
    void SortItems(int (*compare)(const T*, const T*)) {
        std::sort(fItems.begin(), fItems.end(), [compare](const T* a, const T* b) {
            return compare(a, b) < 0;
        });
    }

private:
    std::vector<T*> fItems;
    bool fOwns;
};
#endif
