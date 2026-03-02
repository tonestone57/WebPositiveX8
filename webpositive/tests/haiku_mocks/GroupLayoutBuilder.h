#ifndef _MOCK_GROUP_LAYOUT_BUILDER_H
#define _MOCK_GROUP_LAYOUT_BUILDER_H

#include "GroupLayout.h"
#include "InterfaceDefs.h"

class BGroupLayoutBuilder {
public:
    BGroupLayoutBuilder(orientation orientation, float spacing = 0.0) {}
    BGroupLayoutBuilder& Add(BView* view) { return *this; }
    BGroupLayoutBuilder& Add(BView* view, float weight) { return *this; }
    BGroupLayoutBuilder& AddGroup(orientation orientation, float spacing = 0.0) { return *this; }
    BGroupLayoutBuilder& AddGlue() { return *this; }
    BGroupLayoutBuilder& SetInsets(float left, float top, float right, float bottom) { return *this; }
    BGroupLayoutBuilder& End() { return *this; }
    BView* TopView() { return MY_NULLPTR; }
    operator BView*() { return MY_NULLPTR; }
    operator BLayoutItem*() { return MY_NULLPTR; }
};

namespace BLayoutBuilder {
    template<typename T>
    class Group {
    public:
        Group(T* layout) {}
        Group& Add(BView* view) { return *this; }
        Group& Add(BView* view, float weight) { return *this; }
        Group& AddGroup(orientation orientation, float spacing = 0.0) { return *this; }
        Group& AddGlue() { return *this; }
        Group& SetInsets(float left, float top, float right, float bottom) { return *this; }
        Group& End() { return *this; }
        operator BView*() { return MY_NULLPTR; }
    };
}

#endif
