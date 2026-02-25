/*
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#include "AuthenticationPanel.h"

#include <Button.h>
#include <Catalog.h>
#include <CheckBox.h>
#include <ControlLook.h>
#include <GridLayoutBuilder.h>
#include <GroupLayoutBuilder.h>
#include <Locale.h>
#include <Message.h>
#include <MessageRunner.h>
#include <Screen.h>
#include <SeparatorView.h>
#include <SpaceLayoutItem.h>
#include <StringView.h>
#include <TextControl.h>
#include <stdio.h>

static const uint32 kMsgPanelOK = 'pnok';
static const uint32 kMsgJitter = 'jitr';
static const uint32 kHidePassword = 'hdpw';


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Authentication Panel"

AuthenticationPanel::AuthenticationPanel(BRect parentFrame)
	:
	BWindow(BRect(-1000, -1000, -900, -900),
		B_TRANSLATE("Authentication required"), B_TITLED_WINDOW_LOOK,
		B_MODAL_APP_WINDOW_FEEL, B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE
			| B_NOT_ZOOMABLE | B_CLOSE_ON_ESCAPE | B_AUTO_UPDATE_SIZE_LIMITS),
	m_parentWindowFrame(parentFrame),
	m_usernameTextControl(new BTextControl("user", B_TRANSLATE("Username:"),
		"", nullptr)),
	m_passwordTextControl(new BTextControl("pass", B_TRANSLATE("Password:"),
		"", nullptr)),
	m_hidePasswordCheckBox(new BCheckBox("hide", B_TRANSLATE("Hide password "
		"text"), new BMessage(kHidePassword))),
	m_rememberCredentialsCheckBox(new BCheckBox("remember",
		B_TRANSLATE("Remember username and password for this site"), nullptr)),
	m_okButton(new BButton("ok", B_TRANSLATE("OK"),
		new BMessage(kMsgPanelOK))),
	m_cancelButton(new BButton("cancel", B_TRANSLATE("Cancel"),
		new BMessage(B_QUIT_REQUESTED))),
	m_cancelled(false),
	m_exitSemaphore(create_sem(0, "Authentication Panel")),
	m_jitterRunner(nullptr),
	m_jitterCount(0)
{
}


AuthenticationPanel::~AuthenticationPanel()
{
	delete_sem(m_exitSemaphore);
	delete m_jitterRunner;
}


bool
AuthenticationPanel::QuitRequested()
{
	m_cancelled = true;
	release_sem(m_exitSemaphore);
	return false;
}


void
AuthenticationPanel::MessageReceived(BMessage* message)
{
	switch (message->what) {
	case kMsgPanelOK:
		release_sem(m_exitSemaphore);
		break;
	case kHidePassword:
		// Toggle password visibility using the BTextView workaround.
		_UpdatePasswordVisibility();
		break;
	case kMsgJitter: {
		if (m_jitterCount == 0) {
			UpdateIfNeeded();
			m_originalPos = Frame().LeftTop();
			BMessage message(kMsgJitter);
			m_jitterRunner = new BMessageRunner(BMessenger(this), &message,
				15000, 20);
		}

		const float jitterOffsets[] = { -10, 0, 10, 0 };
		const int32 jitterOffsetCount = sizeof(jitterOffsets) / sizeof(float);

		if (m_jitterCount < 20) {
			float offset = jitterOffsets[m_jitterCount % jitterOffsetCount];
			MoveTo(m_originalPos.x + offset, m_originalPos.y);
			m_jitterCount++;
		} else {
			MoveTo(m_originalPos);
			delete m_jitterRunner;
			m_jitterRunner = nullptr;
			m_jitterCount = 0;
		}
		break;
	}
	default:
		BWindow::MessageReceived(message);
	}
}


void
AuthenticationPanel::_UpdatePasswordVisibility()
{
	bool hide = m_hidePasswordCheckBox->Value() == B_CONTROL_ON;
	BTextView* textView = m_passwordTextControl->TextView();

	if (textView->IsTypingHidden() == hide)
		return;

	// BTextView::HideTyping() is destructive when enabling, as it deletes
	// the current text content in the underlying BTextView. To toggle
	// password visibility without losing user input, we must save the
	// current text and selection range, then restore them after toggling
	// the hiding mode.
	BString text = m_passwordTextControl->Text();
	int32 selectionStart = 0;
	int32 selectionEnd = 0;
	textView->GetSelection(&selectionStart, &selectionEnd);

	textView->HideTyping(hide);

	m_passwordTextControl->SetText(text.String());
	textView->Select(selectionStart, selectionEnd);
}


bool AuthenticationPanel::getAuthentication(const BString& text,
	const BString& previousUser, const BString& previousPass,
	bool previousRememberCredentials, bool badPassword,
	BString& user, BString&  pass, bool* rememberCredentials)
{
	// Configure panel and layout controls.
	rgb_color infoColor = ui_color(B_PANEL_TEXT_COLOR);
	BRect textBounds(0, 0, 250, 200);
	BTextView* textView = new BTextView(textBounds, "text", textBounds,
		be_plain_font, &infoColor, B_FOLLOW_NONE, B_WILL_DRAW
			| B_SUPPORTS_LAYOUT);
	textView->SetViewUIColor(B_PANEL_BACKGROUND_COLOR);
	textView->SetText(text.String());
	textView->MakeEditable(false);
	textView->MakeSelectable(false);

	m_usernameTextControl->SetText(previousUser.String());
	// Ignore the previous password, if it didn't work.
	if (!badPassword)
		m_passwordTextControl->SetText(previousPass.String());
	else
		m_passwordTextControl->SetText("");

	m_hidePasswordCheckBox->SetValue(B_CONTROL_ON);
	_UpdatePasswordVisibility();
	m_rememberCredentialsCheckBox->SetValue(previousRememberCredentials);

	// create layout
	SetLayout(new BGroupLayout(B_VERTICAL, 0.0));
	float spacing = be_control_look->DefaultItemSpacing();
	AddChild(BGroupLayoutBuilder(B_VERTICAL, 0.0)
		.Add(BGridLayoutBuilder(0, spacing)
			.Add(textView, 0, 0, 2)
			.Add(m_usernameTextControl->CreateLabelLayoutItem(), 0, 1)
			.Add(m_usernameTextControl->CreateTextViewLayoutItem(), 1, 1)
			.Add(m_passwordTextControl->CreateLabelLayoutItem(), 0, 2)
			.Add(m_passwordTextControl->CreateTextViewLayoutItem(), 1, 2)
			.Add(BSpaceLayoutItem::CreateGlue(), 0, 3)
			.Add(m_hidePasswordCheckBox, 1, 3)
			.Add(m_rememberCredentialsCheckBox, 0, 4, 2)
			.SetInsets(spacing, spacing, spacing, spacing)
		)
		.Add(new BSeparatorView(B_HORIZONTAL, B_PLAIN_BORDER))
		.Add(BGroupLayoutBuilder(B_HORIZONTAL, spacing)
			.AddGlue()
			.Add(m_cancelButton)
			.Add(m_okButton)
			.SetInsets(spacing, spacing, spacing, spacing)
		)
	);

	float textHeight = textView->LineHeight(0) * textView->CountLines();
	textView->SetExplicitMinSize(BSize(B_SIZE_UNSET, textHeight));

	SetDefaultButton(m_okButton);
	if (badPassword && previousUser.Length())
		m_passwordTextControl->MakeFocus(true);
	else
		m_usernameTextControl->MakeFocus(true);

	if (m_parentWindowFrame.IsValid())
		CenterIn(m_parentWindowFrame);
	else
		CenterOnScreen();

	// Start AuthenticationPanel window thread
	Show();

	// Let the window jitter, if the previous password was invalid
	if (badPassword)
		PostMessage(kMsgJitter);

	// Block calling thread
	// Get the originating window, if it exists, to let it redraw itself.
	BWindow* window = dynamic_cast<BWindow*>
		(BLooper::LooperForThread(find_thread(nullptr)));
	if (window) {
		status_t err;
		for (;;) {
			do {
				err = acquire_sem_etc(m_exitSemaphore, 1, B_RELATIVE_TIMEOUT,
					10000);
				// We've (probably) had our time slice taken away from us
			} while (err == B_INTERRUPTED);

			if (err != B_TIMED_OUT) {
				// Semaphore was finally released or nuked.
				break;
			}
			window->UpdateIfNeeded();
		}
	} else {
		// No window to update, so just hang out until we're done.
		while (acquire_sem(m_exitSemaphore) == B_INTERRUPTED) {
		}
	}

	// AuthenticationPanel wants to quit.
	Lock();

	user = m_usernameTextControl->Text();
	pass = m_passwordTextControl->Text();
	if (rememberCredentials)
		*rememberCredentials = m_rememberCredentialsCheckBox->Value()
			== B_CONTROL_ON;

	bool canceled = m_cancelled;
	Quit();
	// AuthenticationPanel object is TOAST here.
	return !canceled;
}
