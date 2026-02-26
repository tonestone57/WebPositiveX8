#ifndef _RECT_H
#define _RECT_H
#include "SupportDefs.h"
#include "Point.h"
#include <algorithm>

class BRect {
public:
    float left, top, right, bottom;
    BRect() : left(0), top(0), right(-1), bottom(-1) {}
    BRect(float l, float t, float r, float b) : left(l), top(t), right(r), bottom(b) {}
    bool IsValid() const { return left <= right && top <= bottom; }
    void InsetBy(float dx, float dy) { left += dx; right -= dx; top += dy; bottom -= dy; }
    float Width() const { return right - left; }
    float Height() const { return bottom - top; }
    bool Contains(BPoint p) const { return p.x >= left && p.x <= right && p.y >= top && p.y <= bottom; }
    bool Contains(BRect r) const { return r.left >= left && r.right <= right && r.top >= top && r.bottom <= bottom; }
    void OffsetTo(float x, float y) { float w = Width(); float h = Height(); left = x; top = y; right = x + w; bottom = y + h; }
    void OffsetTo(BPoint p) { OffsetTo(p.x, p.y); }
    void OffsetBy(float dx, float dy) { left += dx; right += dx; top += dy; bottom += dy; }
    BPoint LeftTop() const { return BPoint(left, top); }
    BRect InsetByCopy(float dx, float dy) const { BRect r = *this; r.InsetBy(dx, dy); return r; }
    BPoint RightBottom() const { return BPoint(right, bottom); }
    BPoint LeftBottom() const { return BPoint(left, bottom); }
    BPoint RightTop() const { return BPoint(right, top); }

    BRect operator|(const BRect& other) const {
        if (!IsValid()) return other;
        if (!other.IsValid()) return *this;
        return BRect(std::min(left, other.left), std::min(top, other.top),
                     std::max(right, other.right), std::max(bottom, other.bottom));
    }
};
#endif
