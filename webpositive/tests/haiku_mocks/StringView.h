#ifndef _STRING_VIEW_H
#define _STRING_VIEW_H

#include "View.h"
#include "String.h"

class BStringView : public BView {
public:
    BStringView(const char* name, const char* text) : fText(text ? text : "") {}
    BStringView(BRect frame, const char* name, const char* text) : fText(text ? text : "") {}
    void SetText(const char* text) { fText = text; }
    const char* Text() const { return fText.String(); }
    void SetAlignment(alignment align) {}
private:
    BString fText;
};

#endif
