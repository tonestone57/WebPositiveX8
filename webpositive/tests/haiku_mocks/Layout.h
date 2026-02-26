#ifndef _LAYOUT_H
#define _LAYOUT_H
#include "ListView.h"
class BLayout {
public:
    virtual ~BLayout() {}
};
class BTwoDimensionalLayout : public BLayout {};
class BGroupLayout : public BTwoDimensionalLayout {
public:
    BGroupLayout(orientation orientation, float spacing = 0.0) {}
    void AddView(int32 index, BView* view) {}
};
class BLayoutBuilder {
public:
    template<typename ParentBuilder>
    class Group {
    public:
        Group<Group<ParentBuilder>> Add(BView* view) { return *this; }
        Group<Group<ParentBuilder>> AddGlue() { return *this; }
        Group<Group<ParentBuilder>> SetInsets(float left, float top, float right, float bottom) { return *this; }
        operator BView*() { return nullptr; }
    };
};
class BGroupLayoutBuilder {
public:
    BGroupLayoutBuilder(orientation orientation, float spacing = 0.0) {}
    BGroupLayoutBuilder& Add(BView* view) { return *this; }
    BGroupLayoutBuilder& AddGlue() { return *this; }
    BGroupLayoutBuilder& SetInsets(float left, float top, float right, float bottom) { return *this; }
    operator BView*() { return nullptr; }
};
class BSeparatorView : public BView {
public:
    BSeparatorView(orientation orientation, uint32 border = 0) {}
};
#endif
