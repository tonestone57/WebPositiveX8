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

int main() {
    std::set<std::string> names;
    std::set<std::string> shortcuts;

    for (int i = 0; kSearchEngines[i].name != NULL; i++) {
        const char* name = kSearchEngines[i].name;
        const char* url = kSearchEngines[i].url;
        const char* shortcut = kSearchEngines[i].shortcut;

        char msg[256];

        // 1. URL must not be NULL and contain %s
        snprintf(msg, sizeof(msg), "Search engine %s has valid URL", name);
        assert_true(url != NULL, msg);
        if (url != NULL) {
            snprintf(msg, sizeof(msg), "Search engine %s URL contains %%s", name);
            assert_true(strstr(url, "%s") != NULL, msg);
        }

        // 2. Shortcut must not be NULL and end with a space
        snprintf(msg, sizeof(msg), "Search engine %s has valid shortcut", name);
        assert_true(shortcut != NULL, msg);
        if (shortcut != NULL) {
            size_t len = strlen(shortcut);
            snprintf(msg, sizeof(msg), "Search engine %s shortcut ends with space", name);
            assert_true(len > 0 && shortcut[len - 1] == ' ', msg);
        }

        // 3. Uniqueness
        snprintf(msg, sizeof(msg), "Search engine name '%s' is unique", name);
        assert_true(names.find(name) == names.end(), msg);
        names.insert(name);

        if (shortcut != NULL) {
            snprintf(msg, sizeof(msg), "Search engine shortcut '%s' is unique", shortcut);
            assert_true(shortcuts.find(shortcut) == shortcuts.end(), msg);
            shortcuts.insert(shortcut);
        }
    }

    if (gTestFailures > 0) {
        printf("Finished running tests: %d failures\n", gTestFailures);
        return 1;
    }

    printf("All tests passed!\n");
    return 0;
}
