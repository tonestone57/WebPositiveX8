#ifndef _STRING_ITEM_H
#define _STRING_ITEM_H
#include "ListView.h"
#include "String.h"

class BStringItem : public BListItem {
public:
    BStringItem(const char* text, uint32 outlineLevel = 0, bool expanded = false)
        : BListItem(outlineLevel, expanded), fText(text ? text : "") {}
    virtual ~BStringItem() {}
    const char* Text() const { return fText.String(); }
    void SetText(const char* text) { fText = text; }
private:
    BString fText;
};
#endif
