#ifndef _BITMAP_H
#define _BITMAP_H
#include "InterfaceDefs.h"
#include "Rect.h"
#include "Archivable.h"
class BBitmap : public BArchivable {
public:
    BBitmap(BRect bounds, color_space space, bool acceptsViews = false, bool needsContiguous = false) : fBounds(bounds) {}
    BBitmap(BRect bounds, uint32 flags, color_space space, int32 bytesPerRow = -1, screen_id screen = {0}) : fBounds(bounds) {}
    BBitmap(BMessage* archive) {}
    BBitmap(const BBitmap* other) { if (other) fBounds = other->fBounds; }
    virtual ~BBitmap() {}
    BRect Bounds() const { return fBounds; }
    void* Bits() const { return nullptr; }
    int32 BitsLength() const { return 0; }
    status_t Archive(BMessage* archive, bool deep = true) const { return B_OK; }
private:
    BRect fBounds;
};
#endif
