#ifndef _MOCK_SETTINGS_FILE_H
#define _MOCK_SETTINGS_FILE_H
#include "BeOSCompatibility.h"
#include "SupportDefs.h"
#include "File.h"
#include "Path.h"
status_t OpenSettingsFile(BFile& file, const char* fileName, uint32 mode);
status_t GetSettingsPath(BPath& path, const char* fileName = MY_NULLPTR);
#endif
