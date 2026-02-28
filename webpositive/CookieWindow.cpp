/*
 * Copyright 2015 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Adrien Destugues
 */


#include "BeOSCompatibility.h"
#include "CookieWindow.h"

#include <Button.h>
#include <Catalog.h>
#include <ColumnListView.h>
#include <ColumnTypes.h>
#include <GroupLayoutBuilder.h>
#include <NetworkCookieJar.h>
#include <OutlineListView.h>
#include <StringItem.h>
#include <ScrollView.h>
#include <StringView.h>

#include <new>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Cookie Manager"

enum {
	COOKIE_IMPORT = 'cimp',
	COOKIE_EXPORT = 'cexp',
	COOKIE_DELETE = 'cdel',
	COOKIE_REFRESH = 'rfsh',

	DOMAIN_SELECTED = 'dmsl'
};


class CookieDateColumn: public BDateColumn
{
public:
	CookieDateColumn(const char* title, float width)
		:
		BDateColumn(title, width, width / 2, width * 2)
	{
	}

	void DrawField(BField* field, BRect rect, BView* parent) {
		BDateField* dateField = static_cast<BDateField*>(field);
		if (dateField != 0 && dateField->UnixTime() == -1) {
			DrawString(B_TRANSLATE("Session cookie"), parent, rect);
		} else {
			BDateColumn::DrawField(field, rect, parent);
		}
	}
};


class CookieRow: public BRow
{
public:
	CookieRow(BColumnListView* list,
		const BPrivate::Network::BNetworkCookie& cookie)
		:
		BRow(),
		fCookie(cookie)
	{
		list->AddRow(this);
		SetField(new BStringField(cookie.Name().String()), 0);
		SetField(new BStringField(cookie.Path().String()), 1);
		time_t expiration = cookie.ExpirationDate();
		SetField(new BDateField(&expiration), 2);
		SetField(new BStringField(cookie.Value().String()), 3);

		BString flags;
		if (cookie.Secure())
			flags = "https ";
		if (cookie.HttpOnly())
			flags = "http ";

		if (cookie.IsHostOnly())
			flags += "hostOnly";
		SetField(new BStringField(flags.String()), 4);
	}

	BPrivate::Network::BNetworkCookie& Cookie() {
		return fCookie;
	}

private:
	BPrivate::Network::BNetworkCookie	fCookie;
};


class DomainItem: public BStringItem
{
public:
	DomainItem(BString text, bool empty)
		:
		BStringItem(text),
		fEmpty(empty)
	{
	}

public:
	bool	fEmpty;
};


CookieWindow::CookieWindow(BRect frame,
	BPrivate::Network::BNetworkCookieJar& jar)
	:
	BWindow(frame, B_TRANSLATE("Cookie manager"), B_TITLED_WINDOW,
		B_NORMAL_WINDOW_FEEL,
		B_AUTO_UPDATE_SIZE_LIMITS | B_ASYNCHRONOUS_CONTROLS | B_NOT_ZOOMABLE),
	fCookieJar(&jar)
{
	BGroupLayout* root = new BGroupLayout(B_HORIZONTAL, 0.0);
	SetLayout(root);

	fDomains = new BOutlineListView("domain list");
	root->AddView(new BScrollView("scroll", fDomains, 0, false, true), 1);

	fHeaderView = new BStringView("label",
		B_TRANSLATE("The cookie jar is empty!"));
	fCookies = new BColumnListView("cookie list", B_WILL_DRAW, B_FANCY_BORDER,
		false);

	int em = fCookies->StringWidth("M");
	int flagsLength = fCookies->StringWidth("Mhttps hostOnly" B_UTF8_ELLIPSIS);

	fCookies->AddColumn(new BStringColumn(B_TRANSLATE("Name"),
		20 * em, 10 * em, 50 * em, 0), 0);
	fCookies->AddColumn(new BStringColumn(B_TRANSLATE("Path"),
		10 * em, 10 * em, 50 * em, 0), 1);
	fCookies->AddColumn(new CookieDateColumn(B_TRANSLATE("Expiration"),
		fCookies->StringWidth("88/88/8888 88:88:88 AM")), 2);
	fCookies->AddColumn(new BStringColumn(B_TRANSLATE("Value"),
		20 * em, 10 * em, 50 * em, 0), 3);
	fCookies->AddColumn(new BStringColumn(B_TRANSLATE("Flags"),
		flagsLength, flagsLength, flagsLength, 0), 4);

	root->AddItem(BGroupLayoutBuilder(B_VERTICAL, B_USE_DEFAULT_SPACING)
		.SetInsets(5, 5, 5, 5)
		.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING)
			.Add(fHeaderView)
			.AddGlue()
		.End()
		.Add(fCookies)
		.AddGroup(B_HORIZONTAL, B_USE_DEFAULT_SPACING)
			.SetInsets(5, 5, 5, 5)
#if 0
			.Add(new BButton("import", B_TRANSLATE("Import" B_UTF8_ELLIPSIS), 0))
			.Add(new BButton("export", B_TRANSLATE("Export" B_UTF8_ELLIPSIS), 0))
#endif
			.AddGlue()
			.Add(new BButton("delete", B_TRANSLATE("Delete"),
				new BMessage(COOKIE_DELETE))), 3);

	fDomains->SetSelectionMessage(new BMessage(DOMAIN_SELECTED));
}


void
CookieWindow::MessageReceived(BMessage* message)
{
	switch(message->what) {
		case DOMAIN_SELECTED:
		{
			int32 index;
			if (message->FindInt32("index", &index) != B_OK)
				break;
			BStringItem* item = static_cast<BStringItem*>(fDomains->ItemAt(index));
			if (item != MY_NULLPTR) {
				BString domain = item->Text();
				_ShowCookiesForDomain(domain);
			}
			return;
		}

		case COOKIE_REFRESH:
			_BuildDomainList();
			return;

		case COOKIE_DELETE:
			_DeleteCookies();
			return;
	}
	BWindow::MessageReceived(message);
}


void
CookieWindow::Show()
{
	BWindow::Show();
	if (IsHidden())
		return;

	PostMessage(COOKIE_REFRESH);
}


bool
CookieWindow::QuitRequested()
{
	if (!IsHidden())
		Hide();
	return false;
}


CookieWindow::~CookieWindow()
{
	CookieMap::Iterator iterator = fCookieMap.GetIterator();
	while (iterator.HasNext()) {
		delete iterator.Next().value;
	}
}


void
CookieWindow::SetCookieJar(BPrivate::Network::BNetworkCookieJar& jar)
{
	if (Lock()) {
		fCookieJar = &jar;
		if (!IsHidden())
			_BuildDomainList();
		Unlock();
	}
}


void
CookieWindow::_BuildDomainList()
{
	// Empty the domain list (TODO should we do this when hiding instead?)
	for (int i = fDomains->FullListCountItems() - 1; i >= 0; i--) {
		delete fDomains->FullListItemAt(i);
	}
	fDomains->MakeEmpty();

	if (fCookieJar == MY_NULLPTR)
		return;

	// Empty the cookie cache
	CookieMap::Iterator iterator = fCookieMap.GetIterator();
	while (iterator.HasNext()) {
		delete iterator.Next().value;
	}
	fCookieMap.RemoveAll();

	// Populate the domain list and cookie cache
	BPrivate::Network::BNetworkCookieJar::Iterator it = fCookieJar->GetIterator();

	const BPrivate::Network::BNetworkCookie* cookie;
	BString lastDomain;
	bool first = true;
	while ((cookie = it.Next()) != MY_NULLPTR) {
		BString domain = cookie->Domain();
		if (first || domain != lastDomain) {
			_AddDomain(domain, false);
			lastDomain = domain;
			first = false;
		}
		CookieList* list = fCookieMap.Get(domain);
		if (list == MY_NULLPTR) {
			list = new(std::nothrow) CookieList(10, true);
			if (list != MY_NULLPTR)
				fCookieMap.Put(domain, list);
		}
		if (list != MY_NULLPTR)
			list->AddItem(new(std::nothrow) BPrivate::Network::BNetworkCookie(*cookie));
	}

	int i = 0;
	int firstNotEmpty = i;
	// Collapse empty items to keep the list short
	while (i < fDomains->FullListCountItems())
	{
		DomainItem* item = static_cast<DomainItem*>(
			fDomains->FullListItemAt(i));
		if (item != 0 && item->fEmpty == true) {
			if (fDomains->CountItemsUnder(item, true) == 1) {
				// The item has no cookies, and only a single child. We can
				// remove it and move its child one level up in the tree.

				int count = fDomains->CountItemsUnder(item, false);
				int index = fDomains->FullListIndexOf(item) + 1;
				for (int j = 0; j < count; j++) {
					BListItem* child = fDomains->FullListItemAt(index + j);
					child->SetOutlineLevel(child->OutlineLevel() - 1);
				}

				fDomains->RemoveItem(item);
				delete item;

				// The moved child is at the same index the removed item was.
				// We continue the loop without incrementing i to process it.
				continue;
			} else {
				// The item has no cookies, but has multiple children. Mark it
				// as disabled so it is not selectable.
				item->SetEnabled(false);
				if (i == firstNotEmpty)
					firstNotEmpty++;
			}
		}

		i++;
	}

	fDomains->Select(firstNotEmpty);
}


BStringItem*
CookieWindow::_AddDomain(BString domain, bool fake)
{
	BStringItem* parent = 0;
	int firstDot = domain.FindFirst('.');
	if (firstDot >= 0) {
		BString parentDomain(domain);
		parentDomain.Remove(0, firstDot + 1);
		parent = _AddDomain(parentDomain, true);
	}

	int siblingCount = fDomains->CountItemsUnder(parent, true);
	int low = 0;
	int high = siblingCount - 1;
	int insertIndex = siblingCount;

	// check that we aren't already there
	while (low <= high) {
		int mid = (low + high) / 2;
		BStringItem* midItem = static_cast<BStringItem*>(
			fDomains->ItemUnderAt(parent, true, mid));
		if (midItem == MY_NULLPTR)
			break;
		int cmp = strcmp(midItem->Text(), domain.String());
		if (cmp == 0) {
			DomainItem* stringItem = static_cast<DomainItem*>(midItem);
			if (fake == false)
				stringItem->fEmpty = false;
			return stringItem;
		} else if (cmp < 0) {
			low = mid + 1;
		} else {
			insertIndex = mid;
			high = mid - 1;
		}
	}

	// Insert the new item, keeping the list alphabetically sorted
	BStringItem* domainItem = new DomainItem(domain, fake);
	domainItem->SetOutlineLevel(parent != 0 ? parent->OutlineLevel() + 1 : 0);

	if (insertIndex < siblingCount) {
		BListItem* nextSibling = fDomains->ItemUnderAt(parent, true,
			insertIndex);
		fDomains->AddItem(domainItem, fDomains->FullListIndexOf(nextSibling));
	} else {
		if (siblingCount > 0) {
			// There were siblings, but all smaller than what we try to insert.
			// Insert after the last one (and its subitems)
			BListItem* lastSibling = fDomains->ItemUnderAt(parent, true,
				siblingCount - 1);
			fDomains->AddItem(domainItem,
				fDomains->FullListIndexOf(lastSibling)
				+ fDomains->CountItemsUnder(lastSibling, false) + 1);
		} else {
			// There were no siblings, insert right after the parent
			int32 index = parent != 0 ? fDomains->FullListIndexOf(parent) + 1
				: fDomains->FullListCountItems();
			fDomains->AddItem(domainItem, index);
		}
	}

	return domainItem;
}


void
CookieWindow::_ShowCookiesForDomain(BString domain)
{
	BString label;
	label.SetToFormat(B_TRANSLATE("Cookies for %s"), domain.String());
	fHeaderView->SetText(label);

	// Empty the cookie list
	fCookies->Clear();

	// Populate the cookie list from the cache
	CookieList* list = fCookieMap.Get(domain);

	if (list == MY_NULLPTR)
		return;

	for (int32 i = 0; i < list->CountItems(); i++) {
		new CookieRow(fCookies, *list->ItemAt(i));
	}
}


void
CookieWindow::_RemoveCookieFromMap(const BPrivate::Network::BNetworkCookie& cookie)
{
	CookieList* list = fCookieMap.Get(cookie.Domain());
	if (list == MY_NULLPTR)
		return;

	for (int32 i = 0; i < list->CountItems(); i++) {
		BPrivate::Network::BNetworkCookie* c = list->ItemAt(i);
		if (c->Name() == cookie.Name() && c->Path() == cookie.Path()) {
			list->RemoveItem(i);
			break;
		}
	}
}


void
CookieWindow::_DeleteCookies()
{
	if (fCookieJar == MY_NULLPTR)
		return;

	CookieRow* row;
	CookieRow* prevRow;

	for (prevRow = 0; ; prevRow = row) {
		row = static_cast<CookieRow*>(fCookies->CurrentSelection(prevRow));

		if (prevRow != MY_NULLPTR) {
			_RemoveCookieFromMap(prevRow->Cookie());
			fCookies->RemoveRow(prevRow);
			delete prevRow;
		}

		if (row == MY_NULLPTR)
			break;

		// delete this cookie
		BPrivate::Network::BNetworkCookie& cookie = row->Cookie();
		cookie.SetExpirationDate(0);
		fCookieJar->AddCookie(cookie);
	}

	// A domain was selected in the domain list
	if (prevRow == MY_NULLPTR) {
		while (true) {
			// Clear the first cookie continuously
			row = static_cast<CookieRow*>(fCookies->RowAt(0));

			if (row == MY_NULLPTR)
				break;

			BPrivate::Network::BNetworkCookie& cookie = row->Cookie();
			_RemoveCookieFromMap(cookie);
			cookie.SetExpirationDate(0);
			fCookieJar->AddCookie(cookie);
			fCookies->RemoveRow(row);
			delete row;
		}
	}
}
