/*
 * Copyright (C) 2024 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef SETTINGS_FILE_H
#define SETTINGS_FILE_H


#include <SupportDefs.h>


class BFile;


status_t OpenSettingsFile(BFile& file, const char* fileName, uint32 mode);


#endif // SETTINGS_FILE_H
