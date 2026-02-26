#ifndef _CONTROL_LOOK_H
#define _CONTROL_LOOK_H
#include "InterfaceDefs.h"
class BControlLook {
public:
    float DefaultItemSpacing() const { return 5.0f; }
};
extern BControlLook* be_control_look;
#endif
