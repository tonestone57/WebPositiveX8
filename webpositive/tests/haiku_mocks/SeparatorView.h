#ifndef _SEPARATOR_VIEW_H
#define _SEPARATOR_VIEW_H
#include "View.h"
#include "InterfaceDefs.h"
class BSeparatorView : public BView {
public:
    BSeparatorView(orientation orientation, border_style border = B_PLAIN_BORDER) {}
};
#endif
