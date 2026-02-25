/*
 * Copyright (C) 2024 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include "SettingsFile.h"

#include <File.h>
#include <FindDirectory.h>
#include <Path.h>
#include <String.h>
#include <sys/stat.h>

#include "AppConstants.h"


status_t
GetSettingsPath(BPath& path, const char* fileName)
{
	status_t status = find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	if (status != B_OK)
		return status;

	status = path.Append(kApplicationName);
	if (status != B_OK)
		return status;

	if (fileName != NULL)
		status = path.Append(fileName);

	return status;
}


status_t
GetRelativeSettingsPath(BString& path, const char* fileName)
{
	path = kApplicationName;
	if (fileName != NULL) {
		path << "/";
		path << fileName;
	}
	return B_OK;
}


status_t
OpenSettingsFile(BFile& file, const char* fileName, uint32 mode)
{
	BPath path;
	status_t status = GetSettingsPath(path, fileName);
	if (status != B_OK)
		return status;

	status = file.SetTo(path.Path(), mode);
	if (status != B_OK)
		return status;

	if ((mode & B_CREATE_FILE) != 0) {
		status = file.SetPermissions(S_IRUSR | S_IWUSR);
		if (status != B_OK)
			return status;
	}

	return B_OK;
}
