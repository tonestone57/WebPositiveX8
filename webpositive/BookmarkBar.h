/*
 * Copyright 2014, Adrien Destugues <pulkomandy@pulkomandy.tk>.
 * Distributed under the terms of the MIT License.
 */
#ifndef BOOKMARK_BAR_H
#define BOOKMARK_BAR_H


#include "HashMap.h"
#include "HashString.h"

#include <MenuBar.h>
#include <Messenger.h>
#include <Node.h>
#include <NodeMonitor.h>
#include <PopUpMenu.h>
#include <Size.h>


class BBitmap;
class BEntry;

namespace BPrivate {
	class IconMenuItem;
}


class BookmarkBar: public BMenuBar {
	friend class BookmarkBarTest;
public:
									BookmarkBar(const char* title,
										BHandler* target,
										const entry_ref* navDir);
									~BookmarkBar();

	void							AttachedToWindow();
	void							MessageReceived(BMessage* message);

	void							FrameResized(float width, float height);
	BSize							MinSize();

	void 							MouseDown(BPoint where);
private:
	struct LoaderArgs {
		BMessenger messenger;
		node_ref nodeRef;
	};

	void							_AddItem(ino_t inode, BEntry* entry);
	void							_AddItem(ino_t inode, const entry_ref* ref,
										const char* name, bool isDirectory,
										BBitmap* icon);
	static status_t					_LoaderThread(void* data);

private:
	node_ref						fNodeRef;
	HashMap<ino_t, BPrivate::IconMenuItem*> fItemsMap;
	BMenu*							fOverflowMenu;
	// True if fOverflowMenu is currently added to BookmarkBar
	bool							fOverflowMenuAdded;
	BPopUpMenu*						fPopUpMenu;
	int32 							fSelectedItemIndex;
};


#endif // BOOKMARK_BAR_H
