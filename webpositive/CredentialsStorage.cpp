/*
 * Copyright (C) 2010 Stephan Aßmus <superstippi@gmx.de>
 *
 * All rights reserved. Distributed under the terms of the MIT License.
 */

#include "CredentialsStorage.h"

#include <new>
#include <stdio.h>
#include <sys/stat.h>

#include <Autolock.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <KeyStore.h>
#include <Message.h>
#include <OS.h>
#include <Path.h>

#include "BrowserApp.h"
#include "SettingsFile.h"
#include "SettingsKeys.h"


Credentials::Credentials()
	:
	fUsername(),
	fPassword(),
	fIsSecure(false)
{
}


Credentials::Credentials(const BString& username, const BString& password)
	:
	fUsername(username),
	fPassword(password),
	fIsSecure(false)
{
}


Credentials::Credentials(const Credentials& other)
	:
	fUsername(other.fUsername),
	fPassword(other.fPassword),
	fIsSecure(other.fIsSecure)
{
}


Credentials::Credentials(const BMessage* archive)
	:
	fIsSecure(false)
{
	if (archive == NULL)
		return;
	archive->FindString("username", &fUsername);
}


Credentials::~Credentials()
{
}


status_t
Credentials::Archive(BMessage* archive) const
{
	if (archive == NULL)
		return B_BAD_VALUE;
	status_t status = archive->AddString("username", fUsername);
	if (status == B_OK && !fIsSecure && fPassword.Length() > 0)
		status = archive->AddString("password", fPassword);
	return status;
}


Credentials&
Credentials::operator=(const Credentials& other)
{
	if (this == &other)
		return *this;

	fUsername = other.fUsername;
	fPassword = other.fPassword;
	fIsSecure = other.fIsSecure;

	return *this;
}


bool
Credentials::operator==(const Credentials& other) const
{
	if (this == &other)
		return true;

	return fUsername == other.fUsername && fPassword == other.fPassword
		&& fIsSecure == other.fIsSecure;
}


bool
Credentials::operator!=(const Credentials& other) const
{
	return !(*this == other);
}


const BString&
Credentials::Username() const
{
	return fUsername;
}


const BString&
Credentials::Password() const
{
	return fPassword;
}


bool
Credentials::IsSecure() const
{
	return fIsSecure;
}


void
Credentials::SetSecure(bool secure)
{
	fIsSecure = secure;
}


// #pragma mark - CredentialsStorage


CredentialsStorage
CredentialsStorage::sPersistentInstance(true);


CredentialsStorage
CredentialsStorage::sSessionInstance(false);


CredentialsStorage::CredentialsStorage(bool persistent)
	:
	BLocker(persistent ? "persistent credential storage"
		: "credential storage"),
	fCredentialMap(),
	fSettingsLoaded(false),
	fIsLoading(false),
	fPersistent(persistent),
	fQuitting(false),
	fPendingSaveMessage(nullptr),
	fSaveLock("credentials save lock")
{
	fSaveSem = create_sem(0, "credentials save sem");
	fSaveThread = spawn_thread(_SaveThread, "credentials saver",
		B_LOW_PRIORITY, this);
	resume_thread(fSaveThread);
}


CredentialsStorage::~CredentialsStorage()
{
	_SaveSettings();

	fQuitting = true;
	release_sem(fSaveSem);

	status_t exitValue;
	wait_for_thread(fSaveThread, &exitValue);

	delete_sem(fSaveSem);
}


/*static*/ CredentialsStorage*
CredentialsStorage::SessionInstance()
{
	return &sSessionInstance;
}


/*static*/ CredentialsStorage*
CredentialsStorage::PersistentInstance()
{
	sPersistentInstance._EnsureSettingsLoaded();
	return &sPersistentInstance;
}


bool
CredentialsStorage::Contains(const HashString& key)
{
	BAutolock _(this);

	return fCredentialMap.ContainsKey(key);
}


status_t
CredentialsStorage::PutCredentials(const HashString& key,
	const Credentials& credentials)
{
	Credentials updatedCredentials(credentials);

	if (fPersistent) {
		BKeyStore keyStore;
		BString oldUsername;
		bool hadOld = false;

		if (Lock()) {
			if (fCredentialMap.ContainsKey(key)) {
				oldUsername = fCredentialMap.Get(key).Username();
				hadOld = true;
			}
			Unlock();
		}

		if (hadOld) {
			keyStore.RemoveKey(B_KEY_PURPOSE_WEB, key.String(),
				oldUsername.String());
		}

		if (!hadOld || oldUsername != credentials.Username()) {
			keyStore.RemoveKey(B_KEY_PURPOSE_WEB, key.String(),
				credentials.Username().String());
		}

		BPasswordKey passwordKey(credentials.Password().String(),
			B_KEY_PURPOSE_WEB, key.String(), credentials.Username().String());

		if (keyStore.AddKey(passwordKey) == B_OK)
			updatedCredentials.SetSecure(true);
	}

	BAutolock _(this);

	status_t status = fCredentialMap.Put(key, updatedCredentials);
	if (status == B_OK)
		_SaveSettings();

	return status;
}


status_t
CredentialsStorage::RemoveCredentials(const HashString& key)
{
	if (fPersistent) {
		BKeyStore keyStore;
		BString oldUsername;
		bool hadOld = false;

		if (Lock()) {
			if (fCredentialMap.ContainsKey(key)) {
				oldUsername = fCredentialMap.Get(key).Username();
				hadOld = true;
			}
			Unlock();
		}

		if (hadOld) {
			keyStore.RemoveKey(B_KEY_PURPOSE_WEB, key.String(),
				oldUsername.String());
		}
	}

	BAutolock _(this);

	if (!fCredentialMap.ContainsKey(key))
		return B_NAME_NOT_FOUND;

	fCredentialMap.Remove(key);
	_SaveSettings();

	return B_OK;
}


Credentials
CredentialsStorage::GetCredentials(const HashString& key)
{
	BAutolock _(this);

	return fCredentialMap.Get(key);
}


// #pragma mark - private


void
CredentialsStorage::_EnsureSettingsLoaded()
{
	if (!fPersistent || fSettingsLoaded)
		return;

	Lock();
	if (fSettingsLoaded) {
		Unlock();
		return;
	}

	if (fIsLoading) {
		// Wait until loading is finished by another thread
		while (fIsLoading) {
			Unlock();
			snooze(10000);
			Lock();
			if (fSettingsLoaded) {
				Unlock();
				return;
			}
		}
	}

	fIsLoading = true;
	_LoadSettings();
	fIsLoading = false;
	fSettingsLoaded = true;
	Unlock();
}


void
CredentialsStorage::_LoadSettings()
{
	// Called with lock held
	BFile settingsFile;
	BMessage settingsArchive;
	if (OpenSettingsFile(settingsFile, kSettingsFileNameCredentialsStorage,
			B_READ_ONLY) == B_OK) {
		settingsArchive.Unflatten(&settingsFile);
	}

	// Unlock while doing potentially slow IPC to BKeyStore
	Unlock();

	BKeyStore keyStore;
	bool migrationOccurred = false;
	BMessage credentialsArchive;
	for (int32 i = 0; settingsArchive.FindMessage("credentials", i,
			&credentialsArchive) == B_OK; i++) {
		BString key;
		if (credentialsArchive.FindString("key", &key) == B_OK) {
			BString passwordInArchive;
			credentialsArchive.FindString("password", &passwordInArchive);

			Credentials credentials(&credentialsArchive);

			BPasswordKey passwordKey;
			if (keyStore.GetKey(B_KEY_PURPOSE_WEB, key.String(),
					credentials.Username().String(), true, passwordKey)
						== B_OK) {
				credentials = Credentials(credentials.Username(),
					passwordKey.Password());
				credentials.SetSecure(true);
			} else if (passwordInArchive.Length() > 0) {
				// Migration: Save to KeyStore
				BPasswordKey newKey(passwordInArchive.String(),
					B_KEY_PURPOSE_WEB, key.String(),
					credentials.Username().String());
				credentials = Credentials(credentials.Username(),
					passwordInArchive);
				if (keyStore.AddKey(newKey) == B_OK) {
					credentials.SetSecure(true);
					migrationOccurred = true;
				}
			}

			if (credentials.Password().Length() > 0) {
				BAutolock _(this);
				fCredentialMap.Put(key.String(), credentials);
			}
		}
	}

	Lock();
	if (migrationOccurred)
		_SaveSettings();
}


void
CredentialsStorage::_SaveSettings()
{
	// Called with lock held (this)
	if (!fPersistent)
		return;

	std::unique_ptr<BMessage> newMessage(new(std::nothrow) BMessage());
	if (!newMessage)
		return;

	BMessage credentialsArchive;
	CredentialMap::Iterator iterator = fCredentialMap.GetIterator();
	while (iterator.HasNext()) {
		const CredentialMap::Entry& entry = iterator.Next();
		if (entry.value.Archive(&credentialsArchive) != B_OK
			|| credentialsArchive.AddString("key",
				entry.key.String()) != B_OK) {
			credentialsArchive.MakeEmpty();
			continue;
		}
		if (newMessage->AddMessage("credentials",
				&credentialsArchive) != B_OK) {
			credentialsArchive.MakeEmpty();
			continue;
		}
		credentialsArchive.MakeEmpty();
	}

	fSaveLock.Lock();
	fPendingSaveMessage = std::move(newMessage);
	fSaveLock.Unlock();

	release_sem(fSaveSem);
}


status_t
CredentialsStorage::_SaveThread(void* data)
{
	CredentialsStorage* self = (CredentialsStorage*)data;

	while (true) {
		acquire_sem(self->fSaveSem);

		std::unique_ptr<BMessage> messageToSave;

		self->fSaveLock.Lock();
		messageToSave = std::move(self->fPendingSaveMessage);
		self->fSaveLock.Unlock();

		if (self->fQuitting && !messageToSave)
			break;

		if (messageToSave) {
			BFile settingsFile;
			if (OpenSettingsFile(settingsFile, kSettingsFileNameCredentialsStorage,
					B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY) == B_OK) {
				messageToSave->Flatten(&settingsFile);
			}
		}

		if (self->fQuitting)
			break;
	}
	return B_OK;
}



