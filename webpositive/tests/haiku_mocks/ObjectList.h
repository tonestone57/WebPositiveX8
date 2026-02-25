#ifndef _MOCK_OBJECT_LIST_H
#define _MOCK_OBJECT_LIST_H
#include "SupportDefs.h"
template<class T, bool Owns = true>
class BObjectList {
public:
    BObjectList(int32 itemsPerBlock = 20, bool owns = Owns) {}
    ~BObjectList() {}
};
#endif
