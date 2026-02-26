#include "FindDirectory.h"
#include "Clipboard.h"
#include "ControlLook.h"

find_directory_func gMockFindDirectory = nullptr;
BClipboard sClipboard("system");
BClipboard* be_clipboard = &sClipboard;
BControlLook sControlLook;
BControlLook* be_control_look = &sControlLook;
