/*
 * Copyright 2014, Adrien Destugues <pulkomandy@pulkomandy.tk>.
 * Distributed under the terms of the MIT License.
 */


#include "BookmarkBar.h"

#include <Alert.h>
#include <Bitmap.h>
#include <Catalog.h>
#include <ControlLook.h>
#include <Directory.h>
#include <Entry.h>
#include <GroupLayout.h>
#include <IconMenuItem.h>
#include <MessageRunner.h>
#include <NodeInfo.h>
#include <Messenger.h>
#include <PopUpMenu.h>
#include <PromptWindow.h>
#include <TextControl.h>
#include <Window.h>

#include "tracker_private.h"

#include "BrowserWindow.h"
#include "NavMenu.h"

#include <new>
#include <stdio.h>


#define B_TRANSLATION_CONTEXT "BookmarkBar"

const uint32 kOpenNewTabMsg = 'opnt';
const uint32 kDeleteMsg = 'dele';
const uint32 kAskBookmarkNameMsg = 'askn';
const uint32 kShowInTrackerMsg = 'otrk';
const uint32 kRenameBookmarkMsg = 'rena';
const uint32 kFolderMsg = 'fold';
const uint32 kAddBookmarkMsg = 'adbk';


BookmarkBar::BookmarkBar(const char* title, BHandler* target,
	const entry_ref* navDir)
	:
	BMenuBar(title)
{
	SetFlags(Flags() | B_FRAME_EVENTS);
	BEntry(navDir).GetNodeRef(&fNodeRef);

	fOverflowMenu = new BMenu(B_UTF8_ELLIPSIS);
	fOverflowMenuAdded = false;

	fPopUpMenu = new BPopUpMenu("Bookmark Popup", false, false);
	fPopUpMenu->AddItem(
		new BMenuItem(B_TRANSLATE("Open in new tab"), new BMessage(kOpenNewTabMsg)));
	fPopUpMenu->AddItem(new BMenuItem(B_TRANSLATE("Rename"), new BMessage(kAskBookmarkNameMsg)));
	fPopUpMenu->AddItem(
		new BMenuItem(B_TRANSLATE("Show in Tracker"), new BMessage(kShowInTrackerMsg)));
	fPopUpMenu->AddItem(new BSeparatorItem());
	fPopUpMenu->AddItem(new BMenuItem(B_TRANSLATE("Delete"), new BMessage(kDeleteMsg)));
}


BookmarkBar::~BookmarkBar()
{
	stop_watching(BMessenger(this));
	if (!fOverflowMenuAdded)
		delete fOverflowMenu;
	delete fPopUpMenu;
}


void
BookmarkBar::MouseDown(BPoint where)
{
	fSelectedItemIndex = -1;
	BMessage* message = Window()->CurrentMessage();
	if (message != nullptr) {
		int32 buttons = 0;
		if (message->FindInt32("buttons", &buttons) == B_OK) {
			if (buttons & B_SECONDARY_MOUSE_BUTTON) {

				bool foundItem = false;
				for (int32 i = 0; i < CountItems(); i++) {
					BRect itemBounds = ItemAt(i)->Frame();
					if (itemBounds.Contains(where)) {
						foundItem = true;
						fSelectedItemIndex = i;
						break;
					}
				}
				if (foundItem) {
					BPoint screenWhere(where);
					ConvertToScreen(&screenWhere);
					if (ItemAt(fSelectedItemIndex)->Message()->what == kFolderMsg) {
						// This is a directory item, disable "open in new tab"
						fPopUpMenu->ItemAt(0)->SetEnabled(false);
					} else
						fPopUpMenu->ItemAt(0)->SetEnabled(true);

					// Pop up the menu
					fPopUpMenu->SetTargetForItems(this);
					fPopUpMenu->Go(screenWhere, true, true, true);
					return;
				}
			}
		}
	}

	BMenuBar::MouseDown(where);
}


void
BookmarkBar::AttachedToWindow()
{
	BMenuBar::AttachedToWindow();
	watch_node(&fNodeRef, B_WATCH_DIRECTORY, BMessenger(this));

	// Enumerate initial directory content in a background thread
	LoaderArgs* args = new(std::nothrow) LoaderArgs;
	if (args == nullptr)
		return;

	args->messenger = BMessenger(this);
	args->nodeRef = fNodeRef;

	thread_id thread = spawn_thread(_LoaderThread, "BookmarkBar loader",
		B_LOW_PRIORITY, args);
	if (thread < 0)
		delete args;
	else
		resume_thread(thread);
}


void
BookmarkBar::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case kAddBookmarkMsg:
		{
			BBitmap* icon = nullptr;
			message->FindPointer("icon", (void**)&icon);

			ino_t inode;
			entry_ref ref;
			if (message->FindInt64("node", &inode) == B_OK
				&& message->FindRef("ref", &ref) == B_OK) {
				const char* name;
				bool isDirectory;

				if (message->FindString("name", &name) == B_OK
					&& message->FindBool("isDirectory", &isDirectory) == B_OK) {
					_AddItem(inode, &ref, name, isDirectory, icon);
				} else {
					delete icon;
					BEntry entry(&ref);
					_AddItem(inode, &entry);
				}
			} else {
				delete icon;
			}
			break;
		}

		case B_NODE_MONITOR:
		{
			int32 opcode;
			ino_t inode;
			if (message->FindInt32("opcode", &opcode) != B_OK
				|| message->FindInt64("node", &inode) != B_OK)
				break;

			switch (opcode) {
				case B_ENTRY_CREATED:
				{
					entry_ref ref;
					const char* name;

					if (message->FindInt32("device", &ref.device) != B_OK
						|| message->FindInt64("directory", &ref.directory) != B_OK
						|| message->FindString("name", &name) != B_OK)
						break;
					ref.set_name(name);

					BEntry entry(&ref);
					if (entry.InitCheck() == B_OK)
						_AddItem(inode, &entry);
					break;
				}
				case B_ENTRY_MOVED:
				{
					entry_ref ref;
					const char* name;
					ino_t from;

					if (message->FindInt32("device", &ref.device) != B_OK
						|| message->FindInt64("to directory", &ref.directory) != B_OK
						|| message->FindString("name", &name) != B_OK)
						break;
					ref.set_name(name);

					BEntry entry(&ref);
					BEntry followedEntry(&ref, true); // traverse in case it's a symlink

					std::map<ino_t, BPrivate::IconMenuItem*>::iterator it
						= fItemsMap.find(inode);
					if (it == fItemsMap.end()) {
						_AddItem(inode, &entry);
						break;
					} else if (message->FindInt64("from directory", &from) == B_OK
						&& from == (ino_t)ref.directory) {
						// Existing item. Check if it's a rename or a move
						// to some other folder.
						it->second->SetLabel(name);

						BMessage* itemMessage = new BMessage(
							followedEntry.IsDirectory() ? kFolderMsg : B_REFS_RECEIVED);
						itemMessage->AddRef("refs", &ref);
						it->second->SetMessage(itemMessage);

						break;
					}

					// fall through: the item was moved from here to
					// elsewhere, remove it from the bar.
				}
				case B_ENTRY_REMOVED:
				{
					std::map<ino_t, BPrivate::IconMenuItem*>::iterator it
						= fItemsMap.find(inode);
					if (it == fItemsMap.end())
						break;

					IconMenuItem* item = it->second;
					RemoveItem(item);
					fOverflowMenu->RemoveItem(item);
					fItemsMap.erase(it);
					delete item;

					// Reevaluate whether the "more" menu is still needed
					BRect rect = Bounds();
					FrameResized(rect.Width(), rect.Height());
					break;
				}
			}
			break;
		}

		case kOpenNewTabMsg:
		{
			if (fSelectedItemIndex >= 0 && fSelectedItemIndex < CountItems()) {
				// Get the bookmark refs
				entry_ref ref;
				BMenuItem* selectedItem = ItemAt(fSelectedItemIndex);
				if (selectedItem->Message()->what == kFolderMsg
						|| selectedItem->Message()->FindRef("refs", &ref) != B_OK) {
					break;
				}

				// Use the entry_ref to create a BEntry instance and get its path
				BEntry entry(&ref);
				BPath path;
				entry.GetPath(&path);

				BMessage* message = new BMessage(B_REFS_RECEIVED);
				message->AddRef("refs", &ref);
				Window()->PostMessage(message);
			}
			break;
		}
		case kDeleteMsg:
		{
			if (fSelectedItemIndex >= 0 && fSelectedItemIndex < CountItems()) {
				BMenuItem* selectedItem = ItemAt(fSelectedItemIndex);
				// Get the bookmark refs
				entry_ref ref;
				if (selectedItem->Message()->FindRef("refs", &ref) != B_OK)
					break;

				// Use the entry_ref to create a BEntry instance and get its path
				BEntry entry(&ref);
				BPath path;
				entry.GetPath(&path);

				// Remove the bookmark file
				if (entry.Remove() != B_OK) {
					// handle error case if necessary
					BString errorMessage = B_TRANSLATE("Failed to delete bookmark:\n'%path%'");
					errorMessage.ReplaceFirst("%path%", path.Path());
					BAlert* alert = new BAlert("Error", errorMessage.String(), B_TRANSLATE("OK"));
					alert->Go();
					break;
				}

				// Remove the item from the bookmark bar
				if (!RemoveItem(fSelectedItemIndex)) {
					// handle error case if necessary
					BString errorMessage = B_TRANSLATE("Failed to remove bookmark '%leaf%' "
							"from boookmark bar.");
					errorMessage.ReplaceFirst("%leaf%", path.Leaf());
					BAlert* alert = new BAlert("Error", errorMessage.String(), B_TRANSLATE("OK"));
					alert->Go();
				}
			}
			break;
		}
		case kShowInTrackerMsg:
		{
			entry_ref ref;
			if (fSelectedItemIndex >= 0 && fSelectedItemIndex < CountItems()) {
				BMenuItem* selectedItem = ItemAt(fSelectedItemIndex);
				// Get the bookmark refs
				if (selectedItem->Message()->FindRef("refs", &ref) != B_OK)
					break;

				BEntry entry(&ref);
				if (!entry.Exists())
					break;

				node_ref node;
				entry.GetNodeRef(&node);

				BEntry parent;
				entry.GetParent(&parent);
				entry_ref parentRef;
				parent.GetRef(&parentRef);

				// Ask Tracker to open the containing folder and select the
				// file inside it.
				BMessenger trackerMessenger("application/x-vnd.Be-TRAK");
				if (trackerMessenger.IsValid()) {
					BMessage message(B_REFS_RECEIVED);
					message.AddRef("refs", &parentRef);
					message.AddData("nodeRefToSelect", B_RAW_TYPE, &node, sizeof(node_ref));
					trackerMessenger.SendMessage(&message);
				}
			}
			break;
		}
		case kAskBookmarkNameMsg:
		{
			// Get the index of the selected item
			int32 index = fSelectedItemIndex;

			// Get the selected item
			if (index >= 0 && index < CountItems()) {
				BMenuItem* selectedItem = ItemAt(index);
				BString oldName = selectedItem->Label();
				BMessage* message = new BMessage(kRenameBookmarkMsg);
				message->AddPointer("item", selectedItem);
				BString request;
				request.SetToFormat(B_TRANSLATE("Old name: %s"), oldName.String());
				// Create a text control to get the new name from the user
				PromptWindow* prompt = new PromptWindow(B_TRANSLATE("Rename bookmark"),
					B_TRANSLATE("New name:"), request, this, message);
				prompt->Show();
				prompt->CenterOnScreen();
			}
			break;
		}
		case kRenameBookmarkMsg:
		{
			// User clicked OK, get the new name
			BString newName = message->FindString("text");
			BMenuItem* selectedItem = nullptr;
			message->FindPointer("item", (void**)&selectedItem);

			if (selectedItem == nullptr)
				break;

			// Sanitize the name to avoid path traversal
			newName.ReplaceAll('/', '-');
			newName.Truncate(B_FILE_NAME_LENGTH - 1);

			if (newName.IsEmpty())
				break;

			// Rename the bookmark file
			entry_ref ref;
			if (selectedItem->Message()->FindRef("refs", &ref) == B_OK) {
				BEntry entry(&ref);
				if (entry.Rename(newName.String()) == B_OK) {
					// Update the menu item label
					selectedItem->SetLabel(newName);
				}
			}
			break;
		}

		default:
			BMenuBar::MessageReceived(message);
			break;
	}
}


void
BookmarkBar::FrameResized(float width, float height)
{
	int32 count = CountItems();

	// Account for the "more" menu, in terms of item count and space occupied
	int32 overflowMenuWidth = 0;
	if (IndexOf(fOverflowMenu) != B_ERROR) {
		count--;
		// Ignore the width of the "more" menu if it would disappear after
		// removing a bookmark from it.
		if (fOverflowMenu->CountItems() > 1)
			overflowMenuWidth = 32;
	}

	int32 i = 0;
	float rightmost = 0.f;
	while (i < count) {
		BMenuItem* item = ItemAt(i);
		BRect frame = item->Frame();
		if (frame.right > width - overflowMenuWidth)
			break;
		rightmost = frame.right;
		i++;
	}

	if (i == count) {
		// See if we can move some items from the "more" menu in the remaining
		// space.
		BMenuItem* extraItem = fOverflowMenu->ItemAt(0);
		while (extraItem != nullptr) {
			BRect frame = extraItem->Frame();
			if (frame.Width() + rightmost > width - overflowMenuWidth)
				break;
			AddItem(fOverflowMenu->RemoveItem((int32)0), i);
			i++;

			rightmost = ItemAt(i)->Frame().right;
			if (fOverflowMenu->CountItems() <= 1)
				overflowMenuWidth = 0;
			extraItem = fOverflowMenu->ItemAt(0);
		}
		if (fOverflowMenu->CountItems() == 0) {
			RemoveItem(fOverflowMenu);
			fOverflowMenuAdded = false;
		}

	} else {
		// Remove any overflowing item and move them to the "more" menu.
		// Counting backwards avoids complications when indices shift
		// after an item is removed, and keeps bookmarks in the same order,
		// provided they are added at index 0 of the "more" menu.
		for (int j = count - 1; j >= i; j--)
			fOverflowMenu->AddItem(RemoveItem(j), 0);

		if (IndexOf(fOverflowMenu) == B_ERROR) {
			AddItem(fOverflowMenu);
			fOverflowMenuAdded = true;
		}
	}

	BMenuBar::FrameResized(width, height);
}


BSize
BookmarkBar::MinSize()
{
	BSize size = BMenuBar::MinSize();

	// We only need space to show the "more" button.
	size.width = 32;

	// We need enough vertical space to show bookmark icons.
	if (size.height < 20)
		size.height = 20;

	return size;
}


// #pragma mark - private methods


void
BookmarkBar::_AddItem(ino_t inode, BEntry* entry)
{
	// make sure the item doesn't already exist
	if (fItemsMap.find(inode) != fItemsMap.end())
		return;

	entry_ref ref;
	entry->GetRef(&ref);

	char name[B_FILE_NAME_LENGTH];
	entry->GetName(name);

	BEntry followedLink(&ref, true);
	bool isDirectory = followedLink.IsDirectory();

	_AddItem(inode, &ref, name, isDirectory, nullptr);
}


void
BookmarkBar::_AddItem(ino_t inode, const entry_ref* ref, const char* name,
	bool isDirectory, BBitmap* icon)
{
	// make sure the item doesn't already exist
	if (fItemsMap.find(inode) != fItemsMap.end()) {
		delete icon;
		return;
	}

	IconMenuItem* item = nullptr;

	if (isDirectory) {
		delete icon;
		BNavMenu* menu = new BNavMenu(name, B_REFS_RECEIVED, Window());
		menu->SetNavDir(ref);
		BMessage* message = new BMessage(kFolderMsg);
		message->AddRef("refs", ref);
		item = new IconMenuItem(menu, message, "application/x-vnd.Be-directory", B_MINI_ICON);

	} else {
		BMessage* message = new BMessage(B_REFS_RECEIVED);
		message->AddRef("refs", ref);

		if (icon != nullptr) {
			item = new IconMenuItem(name, message, icon, B_MINI_ICON);
			delete icon;
		} else {
			BEntry followedLink(ref, true);
			BNode node(&followedLink);
			BNodeInfo info(&node);
			item = new IconMenuItem(name, message, &info, B_MINI_ICON);
		}
	}

	int32 count = CountItems();
	if (IndexOf(fOverflowMenu) != B_ERROR)
		count--;

	BMenuBar::AddItem(item, count);
	fItemsMap[inode] = item;

	// Move the item to the "more" menu if it overflows.
	BRect rect = Bounds();
	FrameResized(rect.Width(), rect.Height());
}


status_t
BookmarkBar::_LoaderThread(void* data)
{
	LoaderArgs* args = static_cast<LoaderArgs*>(data);
	BDirectory dir(&args->nodeRef);
	BEntry bookmark;
	while (dir.GetNextEntry(&bookmark, false) == B_OK) {
		node_ref nref;
		if (bookmark.GetNodeRef(&nref) == B_OK) {
			BMessage message(kAddBookmarkMsg);
			message.AddInt64("node", nref.node);
			entry_ref eref;
			if (bookmark.GetRef(&eref) == B_OK) {
				message.AddRef("ref", &eref);

				char name[B_FILE_NAME_LENGTH];
				bookmark.GetName(name);
				message.AddString("name", name);

				BEntry followedLink(&eref, true);
				bool isDirectory = followedLink.IsDirectory();
				message.AddBool("isDirectory", isDirectory);

				if (!isDirectory) {
					BNode node(&followedLink);
					BNodeInfo info(&node);
					float iconSize = be_control_look->ComposeIconSize(B_MINI_ICON);
					BBitmap* icon = new(std::nothrow) BBitmap(
						BRect(0, 0, iconSize - 1, iconSize - 1), B_RGBA32);
					if (icon != nullptr) {
						if (info.GetTrackerIcon(icon, B_MINI_ICON) == B_OK) {
							if (message.AddPointer("icon", icon) != B_OK) {
								delete icon;
								icon = nullptr;
							}
						} else {
							delete icon;
							icon = nullptr;
						}
					}
				}

				if (args->messenger.SendMessage(&message) != B_OK) {
					BBitmap* icon = nullptr;
					if (message.FindPointer("icon", (void**)&icon) == B_OK)
						delete icon;
				}
			}
		}
	}
	delete args;
	return B_OK;
}
