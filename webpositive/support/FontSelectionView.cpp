/*
 * Copyright 2001-2010, Haiku.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Mark Hogben
 *		DarkWyrm <bpmagic@columbus.rr.com>
 *		Axel Dörfler, axeld@pinc-software.de
 *		Philippe Saint-Pierre, stpere@gmail.com
 *		Stephan Aßmus <superstippi@gmx.de>
 */

#include "FontSelectionView.h"

#include "private/interface/FontPrivate.h"

#include <Box.h>
#include <Catalog.h>
#include <ControlLook.h>
#include <GroupLayoutBuilder.h>
#include <LayoutItem.h>
#include <Locale.h>
#include <Looper.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <String.h>
#include <TextView.h>

#include <stdio.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Font Selection view"


static const float kMinSize = 8.0;
static const float kMaxSize = 18.0;

static const char* kPreviewText = B_TRANSLATE_COMMENT(
	"The quick brown fox jumps over the lazy dog.",
	"Don't translate this literally ! Use a phrase showing all "
	"chars from A to Z.");

static const int32 kMsgSetFamily = 'fmly';
static const int32 kMsgSetStyle = 'styl';
static const int32 kMsgSetSize = 'size';


//	#pragma mark -


FontSelectionView::FontSelectionView(const char* name, const char* label,
		bool separateStyles, const BFont* currentFont)
	:
	BHandler(name),
	fMessage(nullptr),
	fTarget(nullptr)
{
	if (currentFont == nullptr)
		fCurrentFont = _DefaultFont();
	else
		fCurrentFont = *currentFont;

	fSavedFont = fCurrentFont;

	fSizesMenu = new(std::nothrow) BPopUpMenu("size menu");
	fFontsMenu = new(std::nothrow) BPopUpMenu("font menu");

	// font menu
	fFontsMenuField = new(std::nothrow) BMenuField("fonts", label, fFontsMenu, B_WILL_DRAW);
	if (fFontsMenuField != nullptr)
		fFontsMenuField->SetAlignment(B_ALIGN_RIGHT);

	// styles menu, if desired
	if (separateStyles) {
		fStylesMenu = new(std::nothrow) BPopUpMenu("styles menu");
		fStylesMenuField = new(std::nothrow) BMenuField("styles", B_TRANSLATE("Style:"),
			fStylesMenu, B_WILL_DRAW);
	} else {
		fStylesMenu = nullptr;
		fStylesMenuField = nullptr;
	}

	// size menu
	fSizesMenuField = new(std::nothrow) BMenuField("size", B_TRANSLATE("Size:"), fSizesMenu,
		B_WILL_DRAW);
	if (fSizesMenuField != nullptr)
		fSizesMenuField->SetAlignment(B_ALIGN_RIGHT);

	// preview
	// A string view would be enough if only it handled word-wrap.
	fPreviewTextView = new(std::nothrow) BTextView("preview text");
	if (fPreviewTextView != nullptr) {
		fPreviewTextView->SetFontAndColor(&fCurrentFont);
		fPreviewTextView->SetText(kPreviewText);
		fPreviewTextView->MakeResizable(false);
		fPreviewTextView->SetWordWrap(true);
		fPreviewTextView->MakeEditable(false);
		fPreviewTextView->MakeSelectable(false);
		fPreviewTextView->SetInsets(0, 0, 0, 0);
		fPreviewTextView->SetViewUIColor(B_PANEL_BACKGROUND_COLOR);
		fPreviewTextView->SetHighUIColor(B_PANEL_TEXT_COLOR);

		// determine initial line count using fCurrentFont
		fPreviewTextWidth = be_control_look->DefaultLabelSpacing() * 58.0f;
		float lineCount = ceilf(fCurrentFont.StringWidth(kPreviewText) / fPreviewTextWidth);
		fPreviewTextView->SetExplicitSize(
			BSize(fPreviewTextWidth, fPreviewTextView->LineHeight(0) * lineCount));
	}

	// box around preview
	fPreviewBox = new(std::nothrow) BBox("preview box", B_WILL_DRAW | B_FRAME_EVENTS);
	if (fPreviewBox != nullptr) {
		fPreviewBox->AddChild(BGroupLayoutBuilder(B_VERTICAL)
		.AddGroup(B_HORIZONTAL, 0)
			.Add(fPreviewTextView)
			.AddGlue()
			.End()
			.SetInsets(B_USE_SMALL_SPACING, B_USE_SMALL_SPACING,
				B_USE_SMALL_SPACING, B_USE_SMALL_SPACING)
			.TopView()
		);
	}
	_UpdateFontPreview();
}


FontSelectionView::~FontSelectionView()
{
	// Some controls may not have been attached...
	if (fPreviewTextView != nullptr && !fPreviewTextView->Window())
		delete fPreviewTextView;
	if (fSizesMenuField != nullptr && !fSizesMenuField->Window())
		delete fSizesMenuField;
	if (fStylesMenuField != nullptr && !fStylesMenuField->Window())
		delete fStylesMenuField;
	if (fFontsMenuField != nullptr && !fFontsMenuField->Window())
		delete fFontsMenuField;

	delete fMessage;
}


void
FontSelectionView::AttachedToLooper()
{
	_BuildSizesMenu();
	UpdateFontsMenu();
}


void
FontSelectionView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case B_COLORS_UPDATED:
		{
			if (message->HasColor(ui_color_name(B_PANEL_TEXT_COLOR))) {
				rgb_color textColor;
				if (message->FindColor(ui_color_name(B_PANEL_TEXT_COLOR), &textColor) == B_OK)
					fPreviewTextView->SetFontAndColor(&fCurrentFont, B_FONT_ALL, &textColor);
			}
			break;
		}

		case kMsgSetSize:
		{
			int32 size;
			if (message->FindInt32("size", &size) != B_OK
				|| size == fCurrentFont.Size())
				break;

			fCurrentFont.SetSize(size);
			_UpdateFontPreview();
			_Invoke();
			break;
		}

		case kMsgSetFamily:
		{
			const char* family;
			if (message->FindString("family", &family) != B_OK)
				break;

			font_style style;
			fCurrentFont.GetFamilyAndStyle(nullptr, &style);

			BMenuItem* familyItem = fFontsMenu->FindItem(family);
			if (familyItem != nullptr) {
				_SelectCurrentFont(false);

				BMenuItem* styleItem;
				if (fStylesMenuField != nullptr)
					styleItem = fStylesMenuField->Menu()->FindMarked();
				else {
					styleItem = familyItem->Submenu()->FindItem(style);
					if (styleItem == nullptr)
						styleItem = familyItem->Submenu()->ItemAt(0);
				}

				if (styleItem != nullptr) {
					styleItem->SetMarked(true);
					fCurrentFont.SetFamilyAndStyle(family, styleItem->Label());
					_UpdateFontPreview();
				}
				if (fStylesMenuField != nullptr)
					_AddStylesToMenu(fCurrentFont, fStylesMenuField->Menu());
			}

			_Invoke();
			break;
		}

		case kMsgSetStyle:
		{
			const char* family;
			const char* style;
			if (message->FindString("family", &family) != B_OK
				|| message->FindString("style", &style) != B_OK)
				break;

			BMenuItem *familyItem = fFontsMenu->FindItem(family);
			if (!familyItem)
				break;

			_SelectCurrentFont(false);
			familyItem->SetMarked(true);

			fCurrentFont.SetFamilyAndStyle(family, style);
			_UpdateFontPreview();
			_Invoke();
			break;
		}

		default:
			BHandler::MessageReceived(message);
	}
}


void
FontSelectionView::SetMessage(BMessage* message)
{
	delete fMessage;
	fMessage = message;
}


void
FontSelectionView::SetTarget(BHandler* target)
{
	fTarget = target;
}


// #pragma mark -


void
FontSelectionView::SetFont(const BFont& font, float size)
{
	BFont resizedFont(font);
	resizedFont.SetSize(size);
	SetFont(resizedFont);
}


void
FontSelectionView::SetFont(const BFont& font)
{
	if (font == fCurrentFont && font == fSavedFont)
		return;

	_SelectCurrentFont(false);
	fSavedFont = fCurrentFont = font;
	_UpdateFontPreview();

	_SelectCurrentFont(true);
	_SelectCurrentSize(true);
}


void
FontSelectionView::SetSize(float size)
{
	SetFont(fCurrentFont, size);
}


const BFont&
FontSelectionView::Font() const
{
	return fCurrentFont;
}


void
FontSelectionView::SetDefaults()
{
	BFont defaultFont = _DefaultFont();
	if (defaultFont == fCurrentFont)
		return;

	_SelectCurrentFont(false);

	fCurrentFont = defaultFont;
	_UpdateFontPreview();

	_SelectCurrentFont(true);
	_SelectCurrentSize(true);
}


void
FontSelectionView::Revert()
{
	if (!IsRevertable())
		return;

	_SelectCurrentFont(false);

	fCurrentFont = fSavedFont;
	_UpdateFontPreview();

	_SelectCurrentFont(true);
	_SelectCurrentSize(true);
}


bool
FontSelectionView::IsDefaultable()
{
	return fCurrentFont != _DefaultFont();
}


bool
FontSelectionView::IsRevertable()
{
	return fCurrentFont != fSavedFont;
}


void
FontSelectionView::UpdateFontsMenu()
{
	int32 numFamilies = count_font_families();

	fFontsMenu->RemoveItems(0, fFontsMenu->CountItems(), true);

	BFont font = fCurrentFont;

	font_family currentFamily;
	font_style currentStyle;
	font.GetFamilyAndStyle(&currentFamily, &currentStyle);

	for (int32 i = 0; i < numFamilies; i++) {
		font_family family;
		uint32 flags;
		if (get_font_family(i, &family, &flags) != B_OK)
			continue;

		// if we're setting the fixed font, we only want to show fixed and
		// full-and-half-fixed fonts
		if (strcmp(Name(), "fixed") == 0
			&& (flags & (B_IS_FIXED | B_PRIVATE_FONT_IS_FULL_AND_HALF_FIXED)) == 0) {
			continue;
		}

		font.SetFamilyAndFace(family, B_REGULAR_FACE);

		BMessage* message = new(std::nothrow) BMessage(kMsgSetFamily);
		if (message == nullptr)
			continue;
		message->AddString("family", family);
		message->AddString("name", Name());

		BMenuItem* familyItem = nullptr;
		if (fStylesMenuField != nullptr) {
			familyItem = new(std::nothrow) BMenuItem(family, message);
		} else {
			// Each family item has a submenu with all styles for that font.
			BMenu* stylesMenu = new(std::nothrow) BMenu(family);
			if (stylesMenu != nullptr) {
				_AddStylesToMenu(font, stylesMenu);
				familyItem = new(std::nothrow) BMenuItem(stylesMenu, message);
				if (familyItem == nullptr)
					delete stylesMenu;
			}
		}

		if (familyItem == nullptr) {
			delete message;
			continue;
		}

		familyItem->SetMarked(strcmp(family, currentFamily) == 0);
		if (fFontsMenu != nullptr) {
			if (fFontsMenu->AddItem(familyItem))
				familyItem->SetTarget(this);
			else
				delete familyItem;
		} else
			delete familyItem;
	}

	// Separate styles menu for only the current font.
	if (fStylesMenuField != nullptr)
		_AddStylesToMenu(fCurrentFont, fStylesMenuField->Menu());
}


// #pragma mark - private


BLayoutItem*
FontSelectionView::CreateSizesLabelLayoutItem()
{
	return fSizesMenuField->CreateLabelLayoutItem();
}


BLayoutItem*
FontSelectionView::CreateSizesMenuBarLayoutItem()
{
	return fSizesMenuField->CreateMenuBarLayoutItem();
}


BLayoutItem*
FontSelectionView::CreateFontsLabelLayoutItem()
{
	return fFontsMenuField->CreateLabelLayoutItem();
}


BLayoutItem*
FontSelectionView::CreateFontsMenuBarLayoutItem()
{
	return fFontsMenuField->CreateMenuBarLayoutItem();
}


BLayoutItem*
FontSelectionView::CreateStylesLabelLayoutItem()
{
	if (fStylesMenuField)
		return fStylesMenuField->CreateLabelLayoutItem();
	return nullptr;
}


BLayoutItem*
FontSelectionView::CreateStylesMenuBarLayoutItem()
{
	if (fStylesMenuField)
		return fStylesMenuField->CreateMenuBarLayoutItem();
	return nullptr;
}


BView*
FontSelectionView::PreviewBox() const
{
	return fPreviewBox;
}


// #pragma mark - private


void
FontSelectionView::_Invoke()
{
	if (fTarget != nullptr && fTarget->Looper() != nullptr && fMessage != nullptr) {
		BMessage message(*fMessage);
		fTarget->Looper()->PostMessage(&message, fTarget);
	}
}


BFont
FontSelectionView::_DefaultFont() const
{
	if (strcmp(Name(), "bold") == 0)
		return *be_bold_font;
	if (strcmp(Name(), "fixed") == 0)
		return *be_fixed_font;
	else
		return *be_plain_font;
}


void
FontSelectionView::_SelectCurrentFont(bool select)
{
	font_family family;
	font_style style;
	fCurrentFont.GetFamilyAndStyle(&family, &style);

	BMenuItem *item = fFontsMenu->FindItem(family);
	if (item != nullptr) {
		item->SetMarked(select);

		if (item->Submenu() != nullptr) {
			item = item->Submenu()->FindItem(style);
			if (item != nullptr)
				item->SetMarked(select);
		}
	}
}


void
FontSelectionView::_SelectCurrentSize(bool select)
{
	char label[16];
	snprintf(label, sizeof(label), "%" B_PRId32, (int32)fCurrentFont.Size());

	BMenuItem* item = fSizesMenu->FindItem(label);
	if (item != nullptr)
		item->SetMarked(select);
}


void
FontSelectionView::_UpdateFontPreview()
{
	fPreviewTextView->SetFontAndColor(&fCurrentFont);
	fPreviewTextView->SetExplicitSize(
		BSize(fPreviewTextWidth, fPreviewTextView->LineHeight(0) * fPreviewTextView->CountLines()));
}


void
FontSelectionView::_BuildSizesMenu()
{
	const int32 sizes[] = {7, 8, 9, 10, 11, 12, 13, 14, 18, 21, 24, 0};

	// build size menu
	for (int32 i = 0; sizes[i] != 0; i++) {
		int32 size = sizes[i];
		if (size < kMinSize || size > kMaxSize)
			continue;

		char label[32];
		snprintf(label, sizeof(label), "%" B_PRId32, size);

		BMessage* message = new(std::nothrow) BMessage(kMsgSetSize);
		if (message == nullptr)
			continue;
		message->AddInt32("size", size);
		message->AddString("name", Name());

		BMenuItem* item = new(std::nothrow) BMenuItem(label, message);
		if (item == nullptr) {
			delete message;
			continue;
		}
		if (size == fCurrentFont.Size())
			item->SetMarked(true);

		if (fSizesMenu != nullptr) {
			if (!fSizesMenu->AddItem(item))
				delete item;
			else
				item->SetTarget(this);
		} else
			delete item;
	}
}


void
FontSelectionView::_AddStylesToMenu(const BFont& font, BMenu* stylesMenu) const
{
	stylesMenu->RemoveItems(0, stylesMenu->CountItems(), true);
	stylesMenu->SetRadioMode(true);

	font_family family;
	font_style style;
	font.GetFamilyAndStyle(&family, &style);
	BString currentStyle(style);

	int32 numStyles = count_font_styles(family);

	for (int32 j = 0; j < numStyles; j++) {
		if (get_font_style(family, j, &style) != B_OK)
			continue;

		BMessage* message = new(std::nothrow) BMessage(kMsgSetStyle);
		if (message == nullptr)
			continue;
		message->AddString("family", (char*)family);
		message->AddString("style", (char*)style);

		BMenuItem* item = new(std::nothrow) BMenuItem(style, message);
		if (item == nullptr) {
			delete message;
			continue;
		}
		item->SetMarked(currentStyle == style);

		if (!stylesMenu->AddItem(item))
			delete item;
		else
			item->SetTarget(this);
	}
}

