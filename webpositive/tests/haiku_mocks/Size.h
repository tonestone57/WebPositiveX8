#ifndef _SIZE_H
#define _SIZE_H
#define B_SIZE_UNSET -1.0f
struct BSize {
    float width;
    float height;
    BSize() : width(B_SIZE_UNSET), height(B_SIZE_UNSET) {}
    BSize(float w, float h) : width(w), height(h) {}
};
#endif
