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
#include <Message.h>
#include <Path.h>

#include "BrowserApp.h"
#include "SettingsFile.h"


Credentials::Credentials()
	:
	fUsername(),
	fPassword()
{
}


Credentials::Credentials(const BString& username, const BString& password)
	:
	fUsername(username),
	fPassword(password)
{
}


Credentials::Credentials(const Credentials& other)
{
	*this = other;
}


Credentials::Credentials(const BMessage* archive)
{
	if (archive == nullptr)
		return;
	archive->FindString("username", &fUsername);
	archive->FindString("password", &fPassword);
}


Credentials::~Credentials()
{
}


status_t
Credentials::Archive(BMessage* archive) const
{
	if (archive == nullptr)
		return B_BAD_VALUE;
	status_t status = archive->AddString("username", fUsername);
	if (status == B_OK)
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

	return *this;
}


bool
Credentials::operator==(const Credentials& other) const
{
	if (this == &other)
		return true;

	return fUsername == other.fUsername && fPassword == other.fPassword;
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
	if (sPersistentInstance.Lock()) {
		sPersistentInstance._LoadSettings();
		sPersistentInstance.Unlock();
	}
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
	BAutolock _(this);

	return fCredentialMap.Put(key, credentials);
}


Credentials
CredentialsStorage::GetCredentials(const HashString& key)
{
	BAutolock _(this);

	return fCredentialMap.Get(key);
}


// #pragma mark - private


void
CredentialsStorage::_LoadSettings()
{
	if (!fPersistent || fSettingsLoaded)
		return;

	fSettingsLoaded = true;

	BFile settingsFile;
	if (OpenSettingsFile(settingsFile, "CredentialsStorage", B_READ_ONLY) == B_OK) {
		BMessage settingsArchive;
		settingsArchive.Unflatten(&settingsFile);
		BMessage credentialsArchive;
		for (int32 i = 0; settingsArchive.FindMessage("credentials", i,
				&credentialsArchive) == B_OK; i++) {
			BString key;
			if (credentialsArchive.FindString("key", &key) == B_OK) {
				Credentials credentials(&credentialsArchive);
				fCredentialMap.Put(key.String(), credentials);
			}
		}
	}
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
				entry.key.GetString()) != B_OK) {
			break;
		}
		if (newMessage->AddMessage("credentials",
				&credentialsArchive) != B_OK) {
			break;
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
			if (OpenSettingsFile(settingsFile, "CredentialsStorage",
					B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY) == B_OK) {
				messageToSave->Flatten(&settingsFile);
			}
		}

		if (self->fQuitting)
			break;
	}
	return B_OK;
}



