/*
 * Copyright (C) 2024 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include <stdio.h>
#include <string.h>

#include <Path.h>
#include <FindDirectory.h>
#include <String.h>

// Mocking find_directory using a preprocessor macro before including the source
// This avoids linker conflicts with libbe's find_directory.
static status_t sFindDirectoryResult = B_OK;
static const char* sMockSettingsPath = "/boot/home/config/settings";

#define find_directory mock_find_directory
static status_t
mock_find_directory(directory_which which, BPath* path, bool create_it = false,
	const BVolume* volume = NULL)
{
	if (which == B_USER_SETTINGS_DIRECTORY) {
		if (sFindDirectoryResult == B_OK)
			path->SetTo(sMockSettingsPath);
		return sFindDirectoryResult;
	}
	return B_ERROR;
}

#include "SettingsFile.cpp"
#undef find_directory


// Define kApplicationName for the test as it is typically defined in BrowserApp.cpp
const char* kApplicationName = "WebPositive";

int gTestFailures = 0;


static void
assert_status(status_t expected, status_t actual, const char* message)
{
	if (actual == expected) {
		printf("PASS: %s\n", message);
	} else {
		printf("FAIL: %s (expected %d, got %d)\n", message, (int)expected,
			(int)actual);
		gTestFailures++;
	}
}


static void
assert_string(const char* expected, const char* actual, const char* message)
{
	if ((expected == NULL && actual == NULL) ||
		(expected != NULL && actual != NULL && strcmp(expected, actual) == 0)) {
		printf("PASS: %s\n", message);
	} else {
		printf("FAIL: %s (expected %s, got %s)\n", message,
			expected ? expected : "NULL", actual ? actual : "NULL");
		gTestFailures++;
	}
}


static void
test_get_settings_path_basic()
{
	printf("Testing GetSettingsPath basic functionality...\n");
	BPath path;
	sFindDirectoryResult = B_OK;
	assert_status(B_OK, GetSettingsPath(path, "test.settings"),
		"GetSettingsPath returns B_OK for normal path");

	BString expectedPath(sMockSettingsPath);
	expectedPath << "/" << kApplicationName << "/test.settings";
	assert_string(expectedPath.String(), path.Path(),
		"Path matches expected value (/settings/AppName/filename)");
}


static void
test_get_settings_path_null_filename()
{
	printf("Testing GetSettingsPath with NULL filename...\n");
	BPath path;
	sFindDirectoryResult = B_OK;
	assert_status(B_OK, GetSettingsPath(path, NULL),
		"GetSettingsPath(NULL) returns B_OK");

	BString expectedPath(sMockSettingsPath);
	expectedPath << "/" << kApplicationName;
	assert_string(expectedPath.String(), path.Path(),
		"Path is just the application settings directory");
}


static void
test_get_settings_path_empty_filename()
{
	printf("Testing GetSettingsPath with empty filename...\n");
	BPath path;
	sFindDirectoryResult = B_OK;
	assert_status(B_OK, GetSettingsPath(path, ""),
		"GetSettingsPath(\"\") returns B_OK");

	BString expectedPath(sMockSettingsPath);
	expectedPath << "/" << kApplicationName;
	assert_string(expectedPath.String(), path.Path(),
		"Path is just the application settings directory");
}


static void
test_get_settings_path_error()
{
	printf("Testing GetSettingsPath error handling...\n");
	BPath path;
	sFindDirectoryResult = B_ERROR;
	assert_status(B_ERROR, GetSettingsPath(path, "test.settings"),
		"GetSettingsPath returns error when find_directory fails");
}


int
main()
{
	test_get_settings_path_basic();
	test_get_settings_path_null_filename();
	test_get_settings_path_empty_filename();
	test_get_settings_path_error();

	if (gTestFailures > 0) {
		printf("\nFinished running tests: %d failures\n", gTestFailures);
		return 1;
	}

	printf("\nAll tests passed!\n");
	return 0;
}
