/*
 * Copyright 2015 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Adrien Destugues
 */


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
#include <ObjectList.h>
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
		if (dateField != nullptr && dateField->UnixTime() == -1) {
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
		SetField(new(std::nothrow) BStringField(cookie.Name().String()), 0);
		SetField(new(std::nothrow) BStringField(cookie.Path().String()), 1);
		time_t expiration = cookie.ExpirationDate();
		SetField(new(std::nothrow) BDateField(&expiration), 2);
		SetField(new(std::nothrow) BStringField(cookie.Value().String()), 3);

		BString flags;
		if (cookie.Secure())
			flags = "https ";
		if (cookie.HttpOnly())
			flags = "http ";

		if (cookie.IsHostOnly())
			flags += "hostOnly";
		SetField(new(std::nothrow) BStringField(flags.String()), 4);
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
	BGroupLayout* root = new(std::nothrow) BGroupLayout(B_HORIZONTAL, 0.0);
	SetLayout(root);

	fDomains = new(std::nothrow) BOutlineListView("domain list");
	if (root != nullptr) {
		root->AddView(new(std::nothrow) BScrollView("scroll", fDomains, 0,
			false, true), 1);
	}

	fHeaderView = new(std::nothrow) BStringView("label",
		B_TRANSLATE("The cookie jar is empty!"));
	fCookies = new(std::nothrow) BColumnListView("cookie list", B_WILL_DRAW,
		B_FANCY_BORDER, false);

	if (fCookies != nullptr) {
		int em = fCookies->StringWidth("M");
		int flagsLength = fCookies->StringWidth("Mhttps hostOnly" B_UTF8_ELLIPSIS);

		fCookies->AddColumn(new(std::nothrow) BStringColumn(
			B_TRANSLATE("Name"), 20 * em, 10 * em, 50 * em, 0), 0);
		fCookies->AddColumn(new(std::nothrow) BStringColumn(
			B_TRANSLATE("Path"), 10 * em, 10 * em, 50 * em, 0), 1);
		fCookies->AddColumn(new(std::nothrow) CookieDateColumn(
			B_TRANSLATE("Expiration"), fCookies->StringWidth("88/88/8888 88:88:88 AM")), 2);
		fCookies->AddColumn(new(std::nothrow) BStringColumn(
			B_TRANSLATE("Value"), 20 * em, 10 * em, 50 * em, 0), 3);
		fCookies->AddColumn(new(std::nothrow) BStringColumn(
			B_TRANSLATE("Flags"), flagsLength, flagsLength, flagsLength, 0), 4);
	}

	if (root != nullptr) {
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
			.Add(new(std::nothrow) BButton("delete", B_TRANSLATE("Delete"),
				new(std::nothrow) BMessage(COOKIE_DELETE))), 3);
	}

	if (fDomains != nullptr)
		fDomains->SetSelectionMessage(new(std::nothrow) BMessage(DOMAIN_SELECTED));
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
			if (item != nullptr) {
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
	if (fDomains != nullptr) {
		for (int32 i = fDomains->FullListCountItems() - 1; i >= 0; i--) {
			delete fDomains->FullListItemAt(i);
		}
	}

	for (auto it = fCookieMap.GetIterator(); it.HasNext();) {
		delete it.Next().second;
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

	if (fCookieJar == nullptr)
		return;

	// Empty the cookie cache
	for (auto it = fCookieMap.GetIterator(); it.HasNext();) {
		delete it.Next().second;
	}
	fCookieMap.RemoveAll();
	fDomainMap.RemoveAll();

	// Populate the domain list and cookie cache
	auto it = fCookieJar->GetIterator();

	const BPrivate::Network::BNetworkCookie* cookie;
	BString lastDomain;
	bool first = true;
	while ((cookie = it.Next()) != nullptr) {
		BString domain = cookie->Domain();
		if (first || domain != lastDomain) {
			_AddDomain(domain, false);
			lastDomain = domain;
			first = false;
		}
		CookieList* list = fCookieMap.Get(domain);
		if (list == nullptr) {
			list = new(std::nothrow) CookieList(10, true);
			if (list != nullptr)
				fCookieMap.Put(domain, list);
		}
		if (list != nullptr) {
			BPrivate::Network::BNetworkCookie* copy = new(std::nothrow)
				BPrivate::Network::BNetworkCookie(*cookie);
			if (copy != nullptr)
				list->AddItem(copy);
		}
	}

	int i = 0;
	int firstNotEmpty = i;
	// Collapse empty items to keep the list short
	while (i < fDomains->FullListCountItems())
	{
		DomainItem* item = static_cast<DomainItem*>(
			fDomains->FullListItemAt(i));
		if (item != nullptr && item->fEmpty == true) {
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
	DomainItem* existingItem = fDomainMap.Get(domain);
	if (existingItem != nullptr) {
		if (!fake)
			existingItem->fEmpty = false;
		return existingItem;
	}

	BStringItem* parent = nullptr;
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

	// find insertion point, keeping the list alphabetically sorted
	while (low <= high) {
		int mid = (low + high) / 2;
		BStringItem* midItem = static_cast<BStringItem*>(
			fDomains->ItemUnderAt(parent, true, mid));
		if (midItem == nullptr)
			break;
		int cmp = strcmp(midItem->Text(), domain.String());
		if (cmp < 0) {
			low = mid + 1;
		} else {
			insertIndex = mid;
			high = mid - 1;
		}
	}

	// Insert the new item
	DomainItem* domainItem = new(std::nothrow) DomainItem(domain, fake);
	if (domainItem == nullptr)
		return nullptr;
	domainItem->SetOutlineLevel(parent != nullptr ? parent->OutlineLevel() + 1 : 0);

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
			int32 index = parent != nullptr ? fDomains->FullListIndexOf(parent) + 1
				: fDomains->FullListCountItems();
			fDomains->AddItem(domainItem, index);
		}
	}

	fDomainMap.Put(domain, domainItem);

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

	if (list == nullptr)
		return;

	for (int32 i = 0; i < list->CountItems(); i++) {
		CookieRow* row = new(std::nothrow) CookieRow(fCookies,
			*list->ItemAt(i));
		if (row == nullptr)
			break;
	}
}


void
CookieWindow::_RemoveCookieFromMap(const BPrivate::Network::BNetworkCookie& cookie)
{
	CookieList* list = fCookieMap.Get(cookie.Domain());
	if (list == nullptr)
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
	if (fCookieJar == nullptr)
		return;

	BObjectList<CookieRow> selectedRows(10, false);
	CookieRow* row = nullptr;

	while ((row = static_cast<CookieRow*>(fCookies->CurrentSelection(row)))
			!= nullptr) {
		selectedRows.AddItem(row);
	}

	if (selectedRows.CountItems() > 0) {
		for (int32 i = 0; i < selectedRows.CountItems(); i++) {
			row = selectedRows.ItemAt(i);
			BPrivate::Network::BNetworkCookie& cookie = row->Cookie();
			cookie.SetExpirationDate(0);
			fCookieJar->AddCookie(cookie);

			_RemoveCookieFromMap(cookie);
			fCookies->RemoveRow(row);
			delete row;
		}
	} else {
		// No cookies selected, delete all cookies for the selected domain
		while (fCookies->CountRows() > 0) {
			row = static_cast<CookieRow*>(fCookies->RowAt(0));
			if (row == nullptr)
				break;

			BPrivate::Network::BNetworkCookie& cookie = row->Cookie();
			cookie.SetExpirationDate(0);
			fCookieJar->AddCookie(cookie);

			_RemoveCookieFromMap(cookie);
			fCookies->RemoveRow(row);
			delete row;
		}
	}
}
