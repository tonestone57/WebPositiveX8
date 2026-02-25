/*
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef DOWNLOAD_WINDOW_H
#define DOWNLOAD_WINDOW_H


#include <memory>

#include <Locker.h>
#include <OS.h>
#include <String.h>
#include <Window.h>

class BButton;
class BFile;
class BMessage;
class BGroupLayout;
class BScrollView;
class BWebDownload;
class DownloadProgressView;
class SettingsMessage;


class DownloadWindow : public BWindow {
public:
								DownloadWindow(BRect frame, bool visible,
									SettingsMessage* settings);
	virtual						~DownloadWindow();

	virtual	void				DispatchMessage(BMessage* message,
									BHandler* target);
	virtual void				FrameResized(float newWidth, float newHeight);
	virtual	void				MessageReceived(BMessage* message);
	virtual	bool				QuitRequested();

			bool				DownloadsInProgress();
			void				SetMinimizeOnClose(bool minimize);

private:
			void				_DownloadStarted(BWebDownload* download);
			void				_DownloadFinished(BWebDownload* download);
			void				_RemoveFinishedDownloads();
			void				_RemoveMissingDownloads();
			void				_RemoveDownloads(bool finished, bool missing);
			void				_ValidateButtonStatus();
			void				_SaveSettings();
			void				_LoadSettings();

			int32				_RemoveExistingDownload(const BString& url);
			void				_ScrollToView(DownloadProgressView* view,
									int32 index);

			DownloadProgressView*
								_ViewAt(int32 index) const;
			void				_RemoveView(int32 index);

			DownloadProgressView*
								_FindView(BWebDownload* download);

	static	status_t			_SaveThread(void* data);

private:
			BScrollView*		fDownloadsScrollView;
			BGroupLayout*		fDownloadViewsLayout;
			BButton*			fRemoveFinishedButton;
			BButton*			fRemoveMissingButton;
			BString				fDownloadPath;
			bool				fMinimizeOnClose;

			thread_id			fSaveThread;
			sem_id				fSaveSem;
			bool				fQuitting;
			std::unique_ptr<BMessage>
								fPendingSaveMessage;
			BLocker				fSaveLock;
};

#endif // DOWNLOAD_WINDOW_H
