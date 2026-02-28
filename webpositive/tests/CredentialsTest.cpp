/*
 * Copyright (C) 2024 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include "BeOSCompatibility.h"
#include <stdio.h>

#include <Message.h>
#include <String.h>

#include "CredentialsStorage.h"


const char* kApplicationName = "WebPositive";

int gTestFailures = 0;


static void
assert_true(bool condition, const char* message)
{
	if (condition) {
		printf("PASS: %s\n", message);
	} else {
		printf("FAIL: %s\n", message);
		gTestFailures++;
	}
}


static void
assert_status(status_t expected, status_t actual, const char* message)
{
	if (actual == expected) {
		printf("PASS: %s\n", message);
	} else {
		printf("FAIL: %s (expected %d, got %d)\n", message, (int)expected,
			(int)actual);
		gTestFailures++;
	}
}


int
main()
{
	printf("Testing Credentials class...\n");

	// Default constructor
	{
		Credentials c;
		assert_true(c.Username() == "", "Default username is empty");
		assert_true(c.Password() == "", "Default password is empty");
	}

	// Parameterized constructor
	{
		Credentials c("user", "pass");
		assert_true(c.Username() == "user", "Parameterized username is 'user'");
		assert_true(c.Password() == "pass", "Parameterized password is 'pass'");
	}

	// Copy constructor
	{
		Credentials c1("user", "pass");
		c1.SetSecure(true);
		Credentials c2(c1);
		assert_true(c2.Username() == "user", "Copy username matches");
		assert_true(c2.Password() == "pass", "Copy password matches");
		assert_true(c2.IsSecure() == true, "Copy is secure");
		assert_true(c2 == c1, "Copy is equal to original");
	}

	// BMessage constructor
	{
		BMessage archive;
		archive.AddString("username", "user");
		archive.AddString("password", "pass");
		Credentials c(&archive);
		assert_true(c.Username() == "user", "Unarchived username matches");
		assert_true(c.Password() == "", "Unarchived password is empty");
	}

	// BMessage constructor with MY_NULLPTR
	{
		Credentials c(MY_NULLPTR);
		assert_true(c.Username() == "", "Username from MY_NULLPTR archive is empty");
		assert_true(c.Password() == "", "Password from MY_NULLPTR archive is empty");
	}

	// Archive (insecure)
	{
		Credentials c("user", "pass");
		BMessage archive;
		assert_status(B_OK, c.Archive(&archive), "Archive (insecure) returns B_OK");

		BString username;
		BString password;
		assert_status(B_OK, archive.FindString("username", &username),
			"Archive contains username");
		assert_status(B_OK, archive.FindString("password", &password),
			"Archive (insecure) contains password");
		assert_true(username == "user", "Archived username matches");
		assert_true(password == "pass", "Archived password matches");
	}

	// Archive (secure)
	{
		Credentials c("user", "pass");
		c.SetSecure(true);
		BMessage archive;
		assert_status(B_OK, c.Archive(&archive), "Archive (secure) returns B_OK");

		BString username;
		assert_status(B_OK, archive.FindString("username", &username),
			"Archive contains username");
		BString password;
		assert_status(B_NAME_NOT_FOUND, archive.FindString("password", &password),
			"Archive (secure) does NOT contain password");
		assert_true(username == "user", "Archived username matches");
	}

	// Archive with MY_NULLPTR
	{
		Credentials c("user", "pass");
		assert_status(B_BAD_VALUE, c.Archive(MY_NULLPTR),
			"Archive with MY_NULLPTR returns B_BAD_VALUE");
	}

	// Assignment operator
	{
		Credentials c1("user", "pass");
		Credentials c2;
		c2 = c1;
		assert_true(c2.Username() == "user", "Assigned username matches");
		assert_true(c2.Password() == "pass", "Assigned password matches");
		assert_true(c2 == c1, "Assigned is equal to original");

		// Self assignment
		c2 = c2;
		assert_true(c2.Username() == "user",
			"Self-assignment username remains 'user'");
		assert_true(c2.Password() == "pass",
			"Self-assignment password remains 'pass'");
	}

	// Equality operators
	{
		Credentials c1("user", "pass");
		Credentials c2("user", "pass");
		Credentials c3("other", "pass");
		Credentials c4("user", "other");
		Credentials c5("user", "pass");
		c5.SetSecure(true);

		assert_true(c1 == c2, "c1 == c2");
		assert_true(!(c1 != c2), "!(c1 != c2)");
		assert_true(c1 != c3, "c1 != c3 (different username)");
		assert_true(c1 != c4, "c1 != c4 (different password)");
		assert_true(c1 != c5, "c1 != c5 (different security state)");
		assert_true(!(c1 == c3), "!(c1 == c3)");
	}

	if (gTestFailures > 0) {
		printf("\nFinished running tests: %d failures\n", gTestFailures);
		return 1;
	}

	printf("\nAll tests passed!\n");
	return 0;
}
