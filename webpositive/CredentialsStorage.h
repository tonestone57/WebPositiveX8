/*
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved. Distributed under the terms of the MIT License.
 */
#ifndef CREDENTIAL_STORAGE_H
#define CREDENTIAL_STORAGE_H


#include <memory>

#include <Locker.h>
#include <String.h>

#include "HashMap.h"
#include "HashString.h"


class BFile;
class BMessage;
class BString;


class Credentials {
public:
								Credentials();
								Credentials(const BString& username,
									const BString& password);
								Credentials(
									const Credentials& other);
								Credentials(const BMessage* archive);
								~Credentials();

			status_t			Archive(BMessage* archive) const;

			Credentials&		operator=(const Credentials& other);

			bool				operator==(const Credentials& other) const;
			bool				operator!=(const Credentials& other) const;

			const BString&		Username() const;
			const BString&		Password() const;

private:
			BString				fUsername;
			BString				fPassword;
};


class CredentialsStorage : public BLocker {
public:
	static	CredentialsStorage*	SessionInstance();
	static	CredentialsStorage*	PersistentInstance();

			bool				Contains(const BPrivate::HashString& key);
			status_t			PutCredentials(const HashString& key,
									const Credentials& credentials);
			Credentials			GetCredentials(const HashString& key);

private:
								CredentialsStorage(bool persistent);
	virtual						~CredentialsStorage();

			void				_LoadSettings();
			void				_SaveSettings();
			bool				_OpenSettingsFile(BFile& file,
									uint32 mode) const;
	static	status_t			_SaveThread(void* data);

private:
			typedef HashMap<HashString, Credentials> CredentialMap;
			CredentialMap		fCredentialMap;

	static	CredentialsStorage	sPersistentInstance;
	static	CredentialsStorage	sSessionInstance;
			bool				fSettingsLoaded;
			bool				fPersistent;

			thread_id			fSaveThread;
			sem_id				fSaveSem;
			bool				fQuitting;
			std::unique_ptr<BMessage>
								fPendingSaveMessage;
			BLocker				fSaveLock;
};


#endif // CREDENTIAL_STORAGE_H
