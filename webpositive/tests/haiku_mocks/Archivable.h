#ifndef _ARCHIVABLE_H
#define _ARCHIVABLE_H
#include "SupportDefs.h"
class BMessage;
class BArchivable {
public:
    BArchivable() {}
    virtual ~BArchivable() {}
    virtual status_t Archive(BMessage* archive, bool deep = true) const { return B_OK; }
};
#endif
