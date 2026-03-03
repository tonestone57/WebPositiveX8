/*
 * Copyright 2014-2023 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Zhuowei Zhang
 *		Humdinger
 */
#include "ConsoleWindow.h"

#include <Catalog.h>
#include <Clipboard.h>
#include <Message.h>
#include <Button.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>
#include <StringFormat.h>
#include <TextControl.h>
#include <ListView.h>
#include <ScrollView.h>

#include "BrowserWindow.h"
#include "BrowserApp.h"
#include "WebViewConstants.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Console Window"


enum {
	EVAL_CONSOLE_WINDOW_COMMAND = 'ecwc',
	CLEAR_CONSOLE_MESSAGES = 'ccms'
};


ConsoleWindow::ConsoleWindow(BRect frame)
	:
	BWindow(frame, B_TRANSLATE("Script console"), B_TITLED_WINDOW,
		B_NORMAL_WINDOW_FEEL, B_AUTO_UPDATE_SIZE_LIMITS
			| B_ASYNCHRONOUS_CONTROLS | B_NOT_ZOOMABLE),
	fPreviousText(""),
	fRepeatCounter(0)
{
	BGroupLayout* layout = new(std::nothrow) BGroupLayout(B_VERTICAL, 0.0);
	if (layout != nullptr)
		SetLayout(layout);

	fMessagesListView = new(std::nothrow) BListView("Console messages",
		B_MULTIPLE_SELECTION_LIST);

	BMessage* clearMsg = new(std::nothrow) BMessage(CLEAR_CONSOLE_MESSAGES);
	fClearMessagesButton = new(std::nothrow) BButton(B_TRANSLATE("Clear"),
		clearMsg);
	if (fClearMessagesButton != nullptr)
		fClearMessagesButton->SetEnabled(false);
	else
		delete clearMsg;

	BMessage* copyMsg = new(std::nothrow) BMessage(B_COPY);
	fCopyMessagesButton = new(std::nothrow) BButton(B_TRANSLATE("Copy"),
		copyMsg);
	if (fCopyMessagesButton != nullptr)
		fCopyMessagesButton->SetEnabled(false);
	else
		delete copyMsg;

	if (fMessagesListView == nullptr || fClearMessagesButton == nullptr || fCopyMessagesButton == nullptr)
		return;

	BScrollView* scrollView = new(std::nothrow) BScrollView(
		"Console messages scroll", fMessagesListView, 0, true, true);

	if (layout != nullptr && scrollView != nullptr) {
		BGroupLayoutBuilder(layout)
			.Add(scrollView)
			.Add(BGroupLayoutBuilder(B_HORIZONTAL, B_USE_SMALL_SPACING)
				.AddGlue()
				.Add(fClearMessagesButton)
				.Add(fCopyMessagesButton)
				.AddGlue()
				.SetInsets(0, B_USE_SMALL_SPACING, 0, 0))
			.SetInsets(B_USE_SMALL_SPACING, B_USE_SMALL_SPACING,
				B_USE_SMALL_SPACING, B_USE_SMALL_SPACING)
		;
	}
	if (!frame.IsValid())
		CenterOnScreen();
}


ConsoleWindow::~ConsoleWindow()
{
	if (fMessagesListView != nullptr) {
		for (int32 i = fMessagesListView->CountItems() - 1; i >= 0; i--) {
			delete fMessagesListView->RemoveItem(i);
		}
	}
}


void
ConsoleWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case ADD_CONSOLE_MESSAGE:
		{
			BString source;
			int32 lineNumber;
			int32 columnNumber;
			BString text;
			if (message->FindString("source", &source) != B_OK
				|| message->FindInt32("line", &lineNumber) != B_OK
				|| message->FindInt32("column", &columnNumber) != B_OK
				|| message->FindString("string", &text) != B_OK)
				break;
			BString finalText;
			finalText.SetToFormat("%s:%" B_PRIi32 ":%" B_PRIi32 ": %s\n",
				source.String(), lineNumber, columnNumber, text.String());

			if (finalText == fPreviousText) {
				finalText = "";
				static BStringFormat format(B_TRANSLATE("{0, plural,"
					"one{Last line repeated # time.}"
					"other{Last line repeated # times.}}"));
				format.Format(finalText, ++fRepeatCounter);
				// preserve the repeated line
				if (fRepeatCounter > 1) {
					int32 index = fMessagesListView->CountItems() - 1;
					if (index >= 0) {
						BStringItem* item = static_cast<BStringItem*>(
							fMessagesListView->ItemAt(index));
						if (item != nullptr) {
							item->SetText(finalText.String());
							fMessagesListView->InvalidateItem(index);
							break;
						}
					}
				}
			} else {
				fPreviousText = finalText;
				fRepeatCounter = 0;
			}
			BStringItem* newItem = new(std::nothrow) BStringItem(finalText.String());
			if (newItem != nullptr) {
				if (!fMessagesListView->AddItem(newItem))
					delete newItem;
			}
			break;
		}
		case CLEAR_CONSOLE_MESSAGES:
		{
			fPreviousText = "";
			fRepeatCounter = 0;
			int count = fMessagesListView->CountItems();
			for (int i = count - 1; i >= 0; i--)
				delete fMessagesListView->RemoveItem(i);
			break;
		}
		case B_COPY:
		{
			_CopyToClipboard();
			break;
		}
		default:
			BWindow::MessageReceived(message);
			break;
	}
}


bool
ConsoleWindow::QuitRequested()
{
	if (!IsHidden())
		Hide();
	return false;
}


void
ConsoleWindow::_CopyToClipboard()
{
	BString text;
	int32 index;
	if (fMessagesListView->CurrentSelection() == -1) {
		for (int32 i = 0; i < fMessagesListView->CountItems(); i++) {
			BStringItem* item = static_cast<BStringItem*>(
				fMessagesListView->ItemAt(i));
			if (item != nullptr)
				text << item->Text();
		}
	} else {
		for (int32 i = 0; (index = fMessagesListView->CurrentSelection(i)) >= 0; i++) {
			BStringItem* item = static_cast<BStringItem*>(
				fMessagesListView->ItemAt(index));
			if (item != nullptr)
				text << item->Text();
		}
	}

	ssize_t textLen = text.Length();
	if (be_clipboard->Lock()) {
		be_clipboard->Clear();
		BMessage* clip = be_clipboard->Data();
		if (clip != nullptr) {
			clip->AddData("text/plain", B_MIME_TYPE, text.String(), textLen);
			be_clipboard->Commit();
		}
		be_clipboard->Unlock();
	}
}
