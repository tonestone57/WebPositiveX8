#ifndef _STRING_FORMAT_H
#define _STRING_FORMAT_H
#include "String.h"
class BStringFormat {
public:
    BStringFormat(const BString& format) : fFormat(format) {}
    status_t Format(BString& buffer, int32 value) const {
        if (value == 1)
            buffer = "Last line repeated 1 time.";
        else
            buffer.SetToFormat("Last line repeated %d times.", (int)value);
        return B_OK;
    }
private:
    BString fFormat;
};
#endif
