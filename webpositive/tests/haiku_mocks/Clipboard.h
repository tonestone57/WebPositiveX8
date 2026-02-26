#ifndef _CLIPBOARD_H
#define _CLIPBOARD_H
#include "Message.h"
class BClipboard {
public:
    BClipboard(const char* name, bool lookForExternalChanges = false) {}
    bool Lock() { return true; }
    void Unlock() {}
    void Clear() {}
    void Commit() {}
    BMessage* Data() { return &fData; }
private:
    BMessage fData;
};
extern BClipboard* be_clipboard;
#endif
