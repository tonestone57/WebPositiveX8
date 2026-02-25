/*
 * Copyright (C) 2024 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include <stdio.h>
#include <string.h>

#include <Application.h>
#include <ListView.h>
#include <StringItem.h>
#include <Window.h>

#include "ConsoleWindow.h"

// Define constants if not already defined to allow compilation for testing
#ifndef ADD_CONSOLE_MESSAGE
#define ADD_CONSOLE_MESSAGE 'acms'
#endif

// We include the cpp to access private members and methods for testing.
#include "ConsoleWindow.cpp"

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

static void
assert_string(const char* expected, const char* actual, const char* message)
{
	if (strcmp(expected, actual) == 0) {
		printf("PASS: %s (\"%s\" == \"%s\")\n", message, expected, actual);
	} else {
		printf("FAIL: %s (expected \"%s\", got \"%s\")\n", message, expected, actual);
		gTestFailures++;
	}
}

static void
test_add_message()
{
	printf("Testing ADD_CONSOLE_MESSAGE...\n");
	ConsoleWindow* window = new ConsoleWindow(BRect(0, 0, 100, 100));

	BMessage msg(ADD_CONSOLE_MESSAGE);
	msg.AddString("source", "script.js");
	msg.AddInt32("line", 10);
	msg.AddInt32("column", 5);
	msg.AddString("string", "Hello, world!");

	window->MessageReceived(&msg);

	assert_int32(1, window->fMessagesListView->CountItems(), "One message added");
	BStringItem* item = (BStringItem*)window->fMessagesListView->ItemAt(0);
	assert_string("script.js:10:5: Hello, world!\n", item->Text(), "Message text matches");

	delete window;
}

static void
test_message_repetition()
{
	printf("Testing message repetition...\n");
	ConsoleWindow* window = new ConsoleWindow(BRect(0, 0, 100, 100));

	BMessage msg(ADD_CONSOLE_MESSAGE);
	msg.AddString("source", "script.js");
	msg.AddInt32("line", 10);
	msg.AddInt32("column", 5);
	msg.AddString("string", "Hello, world!");

	// 1st time
	window->MessageReceived(&msg);
	assert_int32(1, window->fMessagesListView->CountItems(), "1st message added");

	// 2nd time (repetition)
	window->MessageReceived(&msg);
	assert_int32(2, window->fMessagesListView->CountItems(), "2nd message added (repetition info)");
	BStringItem* item = (BStringItem*)window->fMessagesListView->ItemAt(1);
	// Note: We expect the default English strings from BStringFormat if translation is missing.
	assert_string("Last line repeated 1 time.", item->Text(), "1st repetition text matches");

	// 3rd time (repetition)
	window->MessageReceived(&msg);
	assert_int32(2, window->fMessagesListView->CountItems(), "Still 2 items after 3rd message");
	item = (BStringItem*)window->fMessagesListView->ItemAt(1);
	assert_string("Last line repeated 2 times.", item->Text(), "2nd repetition text matches");

	delete window;
}

static void
test_different_messages()
{
	printf("Testing different messages...\n");
	ConsoleWindow* window = new ConsoleWindow(BRect(0, 0, 100, 100));

	BMessage msg1(ADD_CONSOLE_MESSAGE);
	msg1.AddString("source", "script.js");
	msg1.AddInt32("line", 10);
	msg1.AddInt32("column", 5);
	msg1.AddString("string", "Message 1");

	BMessage msg2(ADD_CONSOLE_MESSAGE);
	msg2.AddString("source", "script.js");
	msg2.AddInt32("line", 11);
	msg2.AddInt32("column", 5);
	msg2.AddString("string", "Message 2");

	window->MessageReceived(&msg1);
	window->MessageReceived(&msg2);

	assert_int32(2, window->fMessagesListView->CountItems(), "Two different messages added");

	BStringItem* item1 = (BStringItem*)window->fMessagesListView->ItemAt(0);
	assert_string("script.js:10:5: Message 1\n", item1->Text(), "1st message text matches");

	BStringItem* item2 = (BStringItem*)window->fMessagesListView->ItemAt(1);
	assert_string("script.js:11:5: Message 2\n", item2->Text(), "2nd message text matches");

	// Add 1st message again
	window->MessageReceived(&msg1);
	assert_int32(3, window->fMessagesListView->CountItems(), "3rd message (same as 1st) added because not consecutive");
	BStringItem* item3 = (BStringItem*)window->fMessagesListView->ItemAt(2);
	assert_string("script.js:10:5: Message 1\n", item3->Text(), "3rd message text matches");

	delete window;
}

static void
test_clear_messages()
{
	printf("Testing CLEAR_CONSOLE_MESSAGES...\n");
	ConsoleWindow* window = new ConsoleWindow(BRect(0, 0, 100, 100));

	BMessage msg(ADD_CONSOLE_MESSAGE);
	msg.AddString("source", "script.js");
	msg.AddInt32("line", 10);
	msg.AddInt32("column", 5);
	msg.AddString("string", "Hello!");

	window->MessageReceived(&msg);
	assert_int32(1, window->fMessagesListView->CountItems(), "Message added");

	BMessage clearMsg(CLEAR_CONSOLE_MESSAGES);
	window->MessageReceived(&clearMsg);
	assert_int32(0, window->fMessagesListView->CountItems(), "Messages cleared");
	assert_true(window->fPreviousText == "", "fPreviousText cleared");

	delete window;
}

int
main()
{
	BApplication app("application/x-vnd.Haiku-ConsoleWindowTest");

	test_add_message();
	test_message_repetition();
	test_different_messages();
	test_clear_messages();

	if (gTestFailures > 0) {
		printf("\nFinished running tests: %d failures\n", gTestFailures);
		return 1;
	}

	printf("\nAll tests passed!\n");
	return 0;
}
