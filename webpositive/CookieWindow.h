/*
 * Copyright 2015 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Adrien Destugues
 */
#ifndef COOKIE_WINDOW_H
#define COOKIE_WINDOW_H


#include <Window.h>

#include <NetworkCookieJar.h>

#include "HashMap.h"
#include "HashString.h"
#include <ObjectList.h>

#include <String.h>


class BColumnListView;
class BOutlineListView;
class BStringItem;
class BStringView;
class BString;


class CookieWindow : public BWindow {
	friend class CookieWindowTest;
public:
								CookieWindow(BRect frame,
									BPrivate::Network::BNetworkCookieJar& jar);
	virtual	void				MessageReceived(BMessage* message);
	virtual void				Show();
	virtual	bool				QuitRequested();
	virtual						~CookieWindow();

			void				SetCookieJar(
									BPrivate::Network::BNetworkCookieJar& jar);

private:
			void				_BuildDomainList();
			BStringItem*		_AddDomain(BString domain, bool fake);
			void				_ShowCookiesForDomain(BString domain);
			void				_DeleteCookies();
			void				_RemoveCookieFromMap(
									const BPrivate::Network::BNetworkCookie& cookie);

private:
	BOutlineListView*			fDomains;
	BColumnListView*			fCookies;
	BStringView*				fHeaderView;

	BPrivate::Network::BNetworkCookieJar*	fCookieJar;

	typedef BObjectList<BPrivate::Network::BNetworkCookie> CookieList;
typedef HashMap<HashString, CookieList*> CookieMap;
CookieMap fCookieMap;
};


#endif // COOKIE_WINDOW_H
