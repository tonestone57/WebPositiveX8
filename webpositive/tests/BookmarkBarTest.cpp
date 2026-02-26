/*
 * Copyright (C) 2024 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include <stdio.h>
#include <string.h>

#include <Application.h>
#include <Bitmap.h>
#include <Catalog.h>
#include <ControlLook.h>
#include <Entry.h>
#include <MenuItem.h>
#include <Messenger.h>
#include <Window.h>

#include <IconMenuItem.h>

#include "BookmarkBar.h"

// We include the cpp to access private members and methods for testing.
// This also gives us access to private constants like kAddBookmarkMsg.
#include "BookmarkBar.cpp"

class BookmarkBarTest {
public:
	static void test_initialization();
	static void test_min_size();
	static void test_overflow_menu_created();
	static void test_add_item_logic();
	static void test_add_bookmark_message();
	static void test_node_monitor_events();
};

int gTestFailures = 0;

static void
assert_true(bool condition, const char* message)
{
	if (condition) {
		printf("PASS: %s\n", message);
	} else {
		printf("FAIL: %s\n", message);
		gTestFailures++;
	}
}

static void
assert_int32(int32 expected, int32 actual, const char* message)
{
	if (actual == expected) {
		printf("PASS: %s (%d == %d)\n", message, (int)expected, (int)actual);
	} else {
		printf("FAIL: %s (expected %d, got %d)\n", message, (int)expected, (int)actual);
		gTestFailures++;
	}
}


void
BookmarkBarTest::test_initialization()
{
	printf("Testing BookmarkBar initialization...\n");

	entry_ref ref;
	BookmarkBar* bar = new BookmarkBar("Bookmarks", nullptr, &ref);

	printf("DEBUG: bar->Name() = '%s'\n", bar->Name());
	assert_true(strcmp(bar->Name(), "Bookmarks") == 0, "Bar name matches");
	assert_int32(0, bar->CountItems(), "Initial bar is empty");

	delete bar;
}


void
BookmarkBarTest::test_min_size()
{
	printf("Testing BookmarkBar::MinSize()...\n");

	entry_ref ref;
	BookmarkBar* bar = new BookmarkBar("Bookmarks", nullptr, &ref);

	BSize minSize = bar->MinSize();
	assert_int32(32, (int32)minSize.width, "Min width should be 32 (for 'more' button)");
	assert_true(minSize.height >= 20, "Min height should be at least 20 (for icons)");

	delete bar;
}


void
BookmarkBarTest::test_overflow_menu_created()
{
	printf("Testing overflow menu creation...\n");

	entry_ref ref;
	BookmarkBar* bar = new BookmarkBar("Bookmarks", nullptr, &ref);

	// fOverflowMenu is private, but we included BookmarkBar.cpp
	assert_true(bar->fOverflowMenu != nullptr, "Overflow menu is created");
	assert_true(!bar->fOverflowMenuAdded, "Overflow menu initially not added to bar");

	delete bar;
}


void
BookmarkBarTest::test_add_item_logic()
{
	printf("Testing BookmarkBar::_AddItem logic...\n");

	entry_ref ref;
	BookmarkBar* bar = new BookmarkBar("Bookmarks", nullptr, &ref);

	// Add to a window to ensure ConvertToScreen and other UI calls have a context
	BWindow* window = new BWindow(BRect(0, 0, 100, 100), "test", B_TITLED_WINDOW, 0);
	window->AddChild(bar);

	// We can test _AddItem(ino_t inode, const entry_ref* ref, const char* name,
	// bool isDirectory, BBitmap* icon)
	entry_ref itemRef(1, 1, "test_bookmark");
	bar->_AddItem(1234, &itemRef, "test_bookmark", false, nullptr);

	// When an item is added, and the bar width is 0 (it is), it should be moved to
	// the overflow menu.
	assert_int32(1, bar->CountItems(), "One item added to the bar (the overflow menu)");
	assert_true(bar->fOverflowMenuAdded, "Overflow menu should be added because width is 0");
	assert_int32(1, bar->fOverflowMenu->CountItems(), "One item in the overflow menu");
	assert_true(bar->fItemsMap.find(1234) != bar->fItemsMap.end(), "Item added to the map");

	if (window->Lock())
		window->Quit();
}


void
BookmarkBarTest::test_add_bookmark_message()
{
	printf("Testing kAddBookmarkMsg...\n");

	entry_ref ref(1, 1, "bookmarks");
	BookmarkBar* bar = new BookmarkBar("Bookmarks", nullptr, &ref);

	BMessage msg(kAddBookmarkMsg);
	msg.AddInt64("node", 1234);
	entry_ref itemRef(1, 1, "my_bookmark");
	msg.AddRef("ref", &itemRef);
	msg.AddString("name", "My Bookmark");
	msg.AddBool("isDirectory", false);

	bar->MessageReceived(&msg);

	assert_true(bar->fItemsMap.find(1234) != bar->fItemsMap.end(), "Item added via message");
	assert_int32(1, bar->fOverflowMenu->CountItems(), "Item present in overflow menu");

	delete bar;
}


void
BookmarkBarTest::test_node_monitor_events()
{
	printf("Testing Node Monitor events (Moved/Removed)...\n");

	entry_ref ref(1, 1, "bookmarks");
	BookmarkBar* bar = new BookmarkBar("Bookmarks", nullptr, &ref);

	// Add an item first
	entry_ref itemRef(1, 1, "test_bookmark");
	bar->_AddItem(999, &itemRef, "test_bookmark", false, nullptr);
	assert_true(bar->fItemsMap.find(999) != bar->fItemsMap.end(), "Item added");

	// Test B_ENTRY_MOVED (Rename)
	BMessage moved(B_NODE_MONITOR);
	moved.AddInt32("opcode", B_ENTRY_MOVED);
	moved.AddInt64("node", 999);
	moved.AddInt32("device", 1);
	moved.AddInt64("from directory", 1);
	moved.AddInt64("to directory", 1);
	moved.AddString("name", "renamed_bookmark");
	printf("DEBUG: sending moved message opcode=%d node=999 from=1 to=1 name='renamed_bookmark'\n", B_ENTRY_MOVED);

	bar->MessageReceived(&moved);
	if (bar->fItemsMap.find(999) != bar->fItemsMap.end()) {
		printf("DEBUG: item label = '%s'\n", bar->fItemsMap[999]->Label());
	} else {
		printf("DEBUG: item 999 not found in map after moved message\n");
	}
	assert_true(bar->fItemsMap.find(999) != bar->fItemsMap.end() && strcmp(bar->fItemsMap[999]->Label(), "renamed_bookmark") == 0, "Item was renamed");

	// Test B_ENTRY_REMOVED
	BMessage removed(B_NODE_MONITOR);
	removed.AddInt32("opcode", B_ENTRY_REMOVED);
	removed.AddInt64("node", 999);

	bar->MessageReceived(&removed);

	assert_true(bar->fItemsMap.find(999) == bar->fItemsMap.end(), "Item removed from map");
	assert_int32(0, bar->fOverflowMenu->CountItems(), "Item removed from menu");

	delete bar;
}


int
main()
{
	BApplication app("application/x-vnd.Haiku-BookmarkBarTest");

	BookmarkBarTest::test_initialization();
	BookmarkBarTest::test_min_size();
	BookmarkBarTest::test_overflow_menu_created();
	BookmarkBarTest::test_add_item_logic();
	BookmarkBarTest::test_add_bookmark_message();
	BookmarkBarTest::test_node_monitor_events();

	if (gTestFailures > 0) {
		printf("\nFinished running tests: %d failures\n", gTestFailures);
		return 1;
	}

	printf("\nAll tests passed!\n");
	return 0;
}
