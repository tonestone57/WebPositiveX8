/*
 * Copyright (C) 2024 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include <stdio.h>

#include <Application.h>
#include <Bitmap.h>
#include <Handler.h>
#include <Looper.h>
#include <Messenger.h>
#include <View.h>

#include "TabManager.h"


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
		printf("PASS: %s\n", message);
	} else {
		printf("FAIL: %s (expected %d, got %d)\n", message, (int)expected,
			(int)actual);
		gTestFailures++;
	}
}


class TestHandler : public BHandler {
public:
	TestHandler()
		:
		BHandler("test handler"),
		fTabChangedCount(0),
		fCloseTabCount(0),
		fLastTabIndex(-1)
	{
	}

	virtual void
	MessageReceived(BMessage* message)
	{
		switch (message->what) {
			case TAB_CHANGED:
				fTabChangedCount++;
				if (message->FindInt32("tab index", &fLastTabIndex) != B_OK)
					fLastTabIndex = -1;
				break;
			case CLOSE_TAB:
				fCloseTabCount++;
				if (message->FindInt32("tab index", &fLastTabIndex) != B_OK)
					fLastTabIndex = -1;
				break;
			default:
				BHandler::MessageReceived(message);
		}
	}

	int32 fTabChangedCount;
	int32 fCloseTabCount;
	int32 fLastTabIndex;
};


static void
test_add_remove_tabs()
{
	printf("Testing AddTab and RemoveTab...\n");

	BLooper* looper = new BLooper();
	TestHandler* handler = new TestHandler();
	looper->AddHandler(handler);
	looper->Run();

	BMessenger target(handler);
	BMessage newTabMessage('ntab');
	TabManager manager(target, &newTabMessage);

	assert_int32(0, manager.CountTabs(), "Initially 0 tabs");

	BView* view1 = new BView("view1", 0);
	manager.AddTab(view1, "Tab 1");
	assert_int32(1, manager.CountTabs(), "1 tab after AddTab");
	assert_true(manager.HasView(view1), "Manager has view1");
	assert_int32(0, manager.TabForView(view1), "view1 is at index 0");
	assert_true(manager.ViewForTab(0) == view1, "View at index 0 is view1");

	BView* view2 = new BView("view2", 0);
	manager.AddTab(view2, "Tab 2");
	assert_int32(2, manager.CountTabs(), "2 tabs after second AddTab");
	assert_int32(1, manager.TabForView(view2), "view2 is at index 1");

	BView* removedView = manager.RemoveTab(0);
	assert_true(removedView == view1, "Removed view is view1");
	assert_int32(1, manager.CountTabs(), "1 tab after RemoveTab");
	assert_int32(0, manager.TabForView(view2), "view2 is now at index 0");

	delete removedView;

	delete manager.TabGroup();
	delete manager.ContainerView();

	looper->Lock();
	looper->Quit();
	delete handler;
}


static void
test_select_tab()
{
	printf("Testing SelectTab...\n");

	BLooper* looper = new BLooper();
	TestHandler* handler = new TestHandler();
	looper->AddHandler(handler);
	looper->Run();

	BMessenger target(handler);
	BMessage newTabMessage('ntab');
	TabManager manager(target, &newTabMessage);

	manager.AddTab(new BView("v1", 0), "T1");
	manager.AddTab(new BView("v2", 0), "T2");

	manager.SelectTab(1);
	assert_int32(1, manager.SelectedTabIndex(), "Selected tab index is 1");

	// Wait for message delivery
	snooze(50000);

	looper->Lock();
	assert_int32(1, handler->fTabChangedCount, "TAB_CHANGED message received");
	assert_int32(1, handler->fLastTabIndex, "Correct tab index in message");
	looper->Unlock();

	delete manager.TabGroup();
	delete manager.ContainerView();

	looper->Lock();
	looper->Quit();
	delete handler;
}


static void
test_close_tab()
{
	printf("Testing CloseTab...\n");

	BLooper* looper = new BLooper();
	TestHandler* handler = new TestHandler();
	looper->AddHandler(handler);
	looper->Run();

	BMessenger target(handler);
	BMessage newTabMessage('ntab');
	TabManager manager(target, &newTabMessage);

	manager.AddTab(new BView("v1", 0), "T1");

	manager.CloseTab(0);

	// Wait for message delivery
	snooze(50000);

	looper->Lock();
	assert_int32(1, handler->fCloseTabCount, "CLOSE_TAB message received");
	assert_int32(0, handler->fLastTabIndex, "Correct tab index in message");
	looper->Unlock();

	delete manager.TabGroup();
	delete manager.ContainerView();

	looper->Lock();
	looper->Quit();
	delete handler;
}


static void
test_tab_labels()
{
	printf("Testing TabLabel...\n");

	BMessenger target;
	BMessage newTabMessage('ntab');
	TabManager manager(target, &newTabMessage);

	manager.AddTab(new BView("v1", 0), "Initial Label");
	assert_true(manager.TabLabel(0) == "Initial Label",
		"Initial label is correct");

	manager.SetTabLabel(0, "New Label");
	assert_true(manager.TabLabel(0) == "New Label",
		"Updated label is correct");

	delete manager.TabGroup();
	delete manager.ContainerView();
}


int
main()
{
	BApplication app("application/x-vnd.WebPositive-TabManagerTest");

	test_add_remove_tabs();
	test_select_tab();
	test_close_tab();
	test_tab_labels();

	if (gTestFailures > 0) {
		printf("\nFinished running tests: %d failures\n", gTestFailures);
		return 1;
	}

	printf("\nAll tests passed!\n");
	return 0;
}
