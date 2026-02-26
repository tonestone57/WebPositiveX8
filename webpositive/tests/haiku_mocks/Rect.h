#ifndef _MOCK_RECT_H
#define _MOCK_RECT_H
#include "SupportDefs.h"
#include "Point.h"
class BRect {
public:
    BRect(float l=0, float t=0, float r=0, float b=0) : left(l), top(t), right(r), bottom(b) {}
    float left, top, right, bottom;
    bool Contains(BPoint p) const {
        return p.x >= left && p.x <= right && p.y >= top && p.y <= bottom;
    }
    bool IsValid() const { return left <= right && top <= bottom; }
    float Width() const { return right - left; }
    float Height() const { return bottom - top; }
    void OffsetTo(float x, float y) {
        float w = Width();
        float h = Height();
        left = x;
        top = y;
        right = left + w;
        bottom = top + h;
    }
};
#endif
