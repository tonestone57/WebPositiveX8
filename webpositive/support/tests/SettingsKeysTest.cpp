#include <stdio.h>
#include <string.h>
#include <set>
#include <string>
#include "SettingsKeys.h"

int gTestFailures = 0;

void assert_true(bool condition, const char* message) {
    if (condition) {
        printf("PASS: %s\n", message);
    } else {
        printf("FAIL: %s\n", message);
        gTestFailures++;
    }
}

bool is_valid_url(const char* url) {
    if (url == NULL) return false;
    if (strncmp(url, "http://", 7) == 0) return true;
    if (strncmp(url, "https://", 8) == 0) return true;
    if (strncmp(url, "file://", 7) == 0) return true;
    return false;
}

int main() {
    std::set<std::string> names;
    std::set<std::string> urls;
    std::set<std::string> shortcuts;

    printf("Validating kSearchEngines...\n");
    for (int i = 0; kSearchEngines[i].name != NULL; i++) {
        const char* name = kSearchEngines[i].name;
        const char* url = kSearchEngines[i].url;
        const char* shortcut = kSearchEngines[i].shortcut;

        char msg[512];

        // 1. Name must be non-empty
        snprintf(msg, sizeof(msg), "Search engine %d has a name", i);
        assert_true(name != NULL && name[0] != '\0', msg);

        // 2. URL must be valid and contain %s
        snprintf(msg, sizeof(msg), "Search engine %s has valid URL", name);
        assert_true(is_valid_url(url), msg);
        if (url != NULL) {
            snprintf(msg, sizeof(msg), "Search engine %s URL contains %%s", name);
            assert_true(strstr(url, "%s") != NULL, msg);
        }

        // 3. Shortcut must not be NULL and end with a space
        snprintf(msg, sizeof(msg), "Search engine %s has valid shortcut", name);
        assert_true(shortcut != NULL, msg);
        if (shortcut != NULL) {
            size_t len = strlen(shortcut);
            snprintf(msg, sizeof(msg), "Search engine %s shortcut ends with space", name);
            assert_true(len > 0 && shortcut[len - 1] == ' ', msg);
        }

        // 4. Uniqueness
        snprintf(msg, sizeof(msg), "Search engine name '%s' is unique", name);
        assert_true(names.find(name) == names.end(), msg);
        names.insert(name);

        if (url != NULL) {
            snprintf(msg, sizeof(msg), "Search engine URL '%s' is unique", url);
            assert_true(urls.find(url) == urls.end(), msg);
            urls.insert(url);
        }

        if (shortcut != NULL) {
            snprintf(msg, sizeof(msg), "Search engine shortcut '%s' is unique", shortcut);
            assert_true(shortcuts.find(shortcut) == shortcuts.end(), msg);
            shortcuts.insert(shortcut);
        }
    }

    printf("\nValidating default settings constants...\n");

    // Validate kDefaultDownloadPath
    assert_true(kDefaultDownloadPath != NULL, "kDefaultDownloadPath is not NULL");
    if (kDefaultDownloadPath != NULL) {
        size_t len = strlen(kDefaultDownloadPath);
        assert_true(len > 0 && kDefaultDownloadPath[len - 1] == '/',
            "kDefaultDownloadPath ends with a slash");
    }

    // Validate kDefaultStartPageURL
    assert_true(kDefaultStartPageURL != NULL, "kDefaultStartPageURL is not NULL");
    assert_true(is_valid_url(kDefaultStartPageURL), "kDefaultStartPageURL is a valid URL");

    // Validate kDefaultSearchPageURL
    assert_true(kDefaultSearchPageURL != NULL, "kDefaultSearchPageURL is not NULL");
    assert_true(is_valid_url(kDefaultSearchPageURL), "kDefaultSearchPageURL is a valid URL");
    if (kDefaultSearchPageURL != NULL) {
        assert_true(strstr(kDefaultSearchPageURL, "%s") != NULL,
            "kDefaultSearchPageURL contains %%s");
    }

    if (gTestFailures > 0) {
        printf("\nFinished running tests: %d failures\n", gTestFailures);
        return 1;
    }

    printf("\nAll tests passed!\n");
    return 0;
}
