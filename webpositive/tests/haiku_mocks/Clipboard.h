#ifndef _CLIPBOARD_H
#define _CLIPBOARD_H
#include "SupportDefs.h"
#include "Message.h"
class BClipboard {
public:
    BClipboard(const char* name, bool lookForExternalChanges = false);
    bool Lock() { return true; }
    void Unlock() {}
    void Clear() {}
    BMessage* Data() { return &fData; }
    void Commit() {}
private:
    BMessage fData;
};
extern BClipboard* be_clipboard;
#endif
