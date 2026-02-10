/*
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef DOWNLOAD_WINDOW_H
#define DOWNLOAD_WINDOW_H


#include <Locker.h>
#include <OS.h>
#include <String.h>
#include <Window.h>

class BButton;
class BFile;
class BGroupLayout;
class BScrollView;
class BWebDownload;
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
			void				_ValidateButtonStatus();
			void				_SaveSettings();
			void				_LoadSettings();
			bool				_OpenSettingsFile(BFile& file, uint32 mode);

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
			BMessage*			fPendingSaveMessage;
			BLocker				fSaveLock;
};

#endif // DOWNLOAD_WINDOW_H
