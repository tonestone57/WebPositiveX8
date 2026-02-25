/*
 * Copyright 2024 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Jules (AI Agent)
 */


#include <stdio.h>
#include <Application.h>
#include <NetworkCookieJar.h>
#include <OutlineListView.h>
#include <ColumnListView.h>
#include <StringView.h>
#include <Button.h>

// We include the .cpp file to access private members for testing.
// Since the Jamfile includes the parent directory in SEARCH_SOURCE,
// this should be found.
#define private public
#include "CookieWindow.cpp"
#undef private

int gTestFailures = 0;


static void
assert_int32(int32 expected, int32 actual, const char* message)
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
		printf("FAIL: %s (expected '%s', got '%s')\n", message,
			expected ? expected : "NULL", actual ? actual : "NULL");
		gTestFailures++;
	}
}


static void
test_add_domain_hierarchy()
{
	printf("Testing CookieWindow::_AddDomain() hierarchy...\n");

	BPrivate::Network::BNetworkCookieJar jar;
	CookieWindow* window = new CookieWindow(BRect(0, 0, 100, 100), jar);

	// _AddDomain should recursively add domains
	window->_AddDomain("sub.example.com", false);

	// We expect "com", "example.com", and "sub.example.com" to be added and nested.
	// BOutlineListView count should be 3.
	assert_int32(3, window->fDomains->FullListCountItems(), "Full domain list should have 3 items");

	DomainItem* item0 = (DomainItem*)window->fDomains->FullListItemAt(0);
	assert_string("com", item0->Text(), "Root domain should be 'com'");
	assert_int32(0, item0->OutlineLevel(), "'com' should be at level 0");

	DomainItem* item1 = (DomainItem*)window->fDomains->FullListItemAt(1);
	assert_string("example.com", item1->Text(), "Middle domain should be 'example.com'");
	assert_int32(1, item1->OutlineLevel(), "'example.com' should be at level 1");

	DomainItem* item2 = (DomainItem*)window->fDomains->FullListItemAt(2);
	assert_string("sub.example.com", item2->Text(), "Leaf domain should be 'sub.example.com'");
	assert_int32(2, item2->OutlineLevel(), "'sub.example.com' should be at level 2");

	window->Lock();
	window->Quit();
}


static void
test_build_domain_list()
{
	printf("Testing CookieWindow::_BuildDomainList()...\n");

	BPrivate::Network::BNetworkCookieJar jar;
	// Use BMessage to populate the jar. This follows the structure used in
	// BNetworkCookieJar::Archive/Unarchive.
	BMessage archive;
	BMessage cookie1;
	cookie1.AddString("name", "name1");
	cookie1.AddString("value", "value1");
	cookie1.AddString("domain", "example.com");
	cookie1.AddString("path", "/");

	BMessage cookie2;
	cookie2.AddString("name", "name2");
	cookie2.AddString("value", "value2");
	cookie2.AddString("domain", "sub.example.com");
	cookie2.AddString("path", "/");

	BMessage cookies;
	cookies.AddMessage("cookie", &cookie1);
	cookies.AddMessage("cookie", &cookie2);
	archive.AddMessage("cookies", &cookies);

	jar.Unarchive(&archive);

	CookieWindow* window = new CookieWindow(BRect(0, 0, 100, 100), jar);

	window->_BuildDomainList();

	// We expect "com", "example.com" and "sub.example.com"
	assert_int32(3, window->fDomains->FullListCountItems(), "Domain list should have 3 items after build");

	DomainItem* item1 = (DomainItem*)window->fDomains->FullListItemAt(1);
	assert_string("example.com", item1->Text(), "Second item should be example.com");

	// Check if cookie map is populated
	assert_int32(1, (int32)window->fCookieMap["example.com"].size(), "example.com should have 1 cookie in map");
	assert_int32(1, (int32)window->fCookieMap["sub.example.com"].size(), "sub.example.com should have 1 cookie in map");

	window->Lock();
	window->Quit();
}


int
main()
{
	// We need a BApplication for UI classes to work
	BApplication app("application/x-vnd.Haiku-CookieWindowTest");

	test_add_domain_hierarchy();
	test_build_domain_list();

	if (gTestFailures > 0) {
		printf("\nFinished running tests: %d failures\n", gTestFailures);
		return 1;
	}

	printf("\nAll tests passed!\n");
	return 0;
}
