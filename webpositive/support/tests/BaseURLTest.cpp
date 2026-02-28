#include "BeOSCompatibility.h"
#include <stdio.h>
#include <stdlib.h>
#include "BaseURL.h"

int gTestFailures = 0;

void assert_string(const char* expected, BString actual, const char* message) {
    if (actual == expected) {
        printf("PASS: %s\n", message);
    } else {
        printf("FAIL: %s (expected '%s', got '%s')\n", message, expected, actual.String());
        gTestFailures++;
    }
}

int main() {
    // Standard cases
    assert_string("www.google.com", baseURL("http://www.google.com/search?q=haiku"), "Standard HTTP URL");
    assert_string("www.google.com", baseURL("http://www.google.com/"), "HTTP URL with trailing slash");
    assert_string("www.google.com", baseURL("http://www.google.com"), "HTTP URL without trailing slash");
    assert_string("github.com", baseURL("https://github.com/haiku/haiku"), "Standard HTTPS URL");
    assert_string("haiku-os.org", baseURL("ftp://haiku-os.org/pub"), "FTP URL");
    assert_string("www.google.com", baseURL("http://www.google.com/a/b/c"), "URL with multiple path segments");

    // Edge cases
    assert_string("", baseURL("://"), "Only protocol marker");
    assert_string("", baseURL("google.com"), "No protocol marker");
    assert_string("", baseURL(""), "Empty string");

    if (gTestFailures > 0) {
        printf("Finished running tests: %d failures\n", gTestFailures);
        return 1;
    }

    printf("All tests passed!\n");
    return 0;
}
