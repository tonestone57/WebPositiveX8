#include "BeOSCompatibility.h"
#include <stdio.h>
#include <string.h>
#include <set>
#include <string>
#include <vector>
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
    if (url == MY_NULLPTR) return false;
    if (strncmp(url, "http://", 7) == 0 && url[7] != '\0') return true;
    if (strncmp(url, "https://", 8) == 0 && url[8] != '\0') return true;
    if (strncmp(url, "file://", 7) == 0 && url[7] != '\0') return true;
    return false;
}

int main() {
    std::set<std::string> names;
    std::set<std::string> urls;
    std::set<std::string> shortcuts;

    printf("Validating kSearchEngines...\n");
    for (int i = 0; kSearchEngines[i].name != 0; i++) {
        const char* name = kSearchEngines[i].name;
        const char* url = kSearchEngines[i].url;
        const char* shortcut = kSearchEngines[i].shortcut;

        char msg[512];

        // 1. Name must be non-empty
        snprintf(msg, sizeof(msg), "Search engine %d has a name", i);
        assert_true(name != 0 && name[0] != '\0', msg);

        // 2. URL must be valid and contain %s
        snprintf(msg, sizeof(msg), "Search engine %s has valid URL", name);
        assert_true(is_valid_url(url), msg);
        if (url != MY_NULLPTR) {
            snprintf(msg, sizeof(msg), "Search engine %s URL contains %%s", name);
            assert_true(strstr(url, "%s") != MY_NULLPTR, msg);
        }

        // 3. Shortcut must not be MY_NULLPTR and end with a space
        snprintf(msg, sizeof(msg), "Search engine %s has valid shortcut", name);
        assert_true(shortcut != MY_NULLPTR, msg);
        if (shortcut != MY_NULLPTR) {
            size_t len = strlen(shortcut);
            snprintf(msg, sizeof(msg), "Search engine %s shortcut ends with space", name);
            assert_true(len > 0 && shortcut[len - 1] == ' ', msg);
        }

        // 4. Uniqueness
        snprintf(msg, sizeof(msg), "Search engine name '%s' is unique", name);
        assert_true(names.find(name) == names.end(), msg);
        names.insert(name);

        if (url != MY_NULLPTR) {
            snprintf(msg, sizeof(msg), "Search engine URL '%s' is unique", url);
            assert_true(urls.find(url) == urls.end(), msg);
            urls.insert(url);
        }

        if (shortcut != MY_NULLPTR) {
            snprintf(msg, sizeof(msg), "Search engine shortcut '%s' is unique", shortcut);
            assert_true(shortcuts.find(shortcut) == shortcuts.end(), msg);
            shortcuts.insert(shortcut);
        }
    }

    printf("\nValidating default settings constants...\n");

    // Validate kDefaultDownloadPath
    assert_true(kDefaultDownloadPath != MY_NULLPTR, "kDefaultDownloadPath is not MY_NULLPTR");
    if (kDefaultDownloadPath != MY_NULLPTR) {
        size_t len = strlen(kDefaultDownloadPath);
        assert_true(len > 0 && kDefaultDownloadPath[len - 1] == '/',
            "kDefaultDownloadPath ends with a slash");
    }

    // Validate kDefaultStartPageURL
    assert_true(kDefaultStartPageURL != MY_NULLPTR, "kDefaultStartPageURL is not MY_NULLPTR");
    assert_true(is_valid_url(kDefaultStartPageURL), "kDefaultStartPageURL is a valid URL");

    // Validate kDefaultSearchPageURL
    assert_true(kDefaultSearchPageURL != MY_NULLPTR, "kDefaultSearchPageURL is not MY_NULLPTR");
    assert_true(is_valid_url(kDefaultSearchPageURL), "kDefaultSearchPageURL is a valid URL");
    if (kDefaultSearchPageURL != MY_NULLPTR) {
        assert_true(strstr(kDefaultSearchPageURL, "%s") != MY_NULLPTR,
            "kDefaultSearchPageURL contains %%s");
    }

    printf("\nValidating settings keys...\n");
    struct KeyInfo {
        const char* name;
        const char* value;
    };
    std::vector<KeyInfo> keys = {
        {"kSettingsKeyDownloadPath", kSettingsKeyDownloadPath},
        {"kSettingsKeyShowTabsIfSinglePageOpen", kSettingsKeyShowTabsIfSinglePageOpen},
        {"kSettingsKeyAutoHideInterfaceInFullscreenMode", kSettingsKeyAutoHideInterfaceInFullscreenMode},
        {"kSettingsKeyAutoHidePointer", kSettingsKeyAutoHidePointer},
        {"kSettingsKeyShowHomeButton", kSettingsKeyShowHomeButton},
        {"kSettingsKeyStartUpPolicy", kSettingsKeyStartUpPolicy},
        {"kSettingsKeyNewWindowPolicy", kSettingsKeyNewWindowPolicy},
        {"kSettingsKeyNewTabPolicy", kSettingsKeyNewTabPolicy},
        {"kSettingsKeyStartPageURL", kSettingsKeyStartPageURL},
        {"kSettingsKeySearchPageURL", kSettingsKeySearchPageURL},
        {"kSettingsKeyUseProxy", kSettingsKeyUseProxy},
        {"kSettingsKeyProxyAddress", kSettingsKeyProxyAddress},
        {"kSettingsKeyProxyPort", kSettingsKeyProxyPort},
        {"kSettingsKeyUseProxyAuth", kSettingsKeyUseProxyAuth},
        {"kSettingsKeyProxyUsername", kSettingsKeyProxyUsername},
        {"kSettingsKeyProxyPassword", kSettingsKeyProxyPassword},
        {"kSettingsShowBookmarkBar", kSettingsShowBookmarkBar}
    };

    std::set<std::string> keyValues;
    for (const auto& key : keys) {
        char msg[512];
        snprintf(msg, sizeof(msg), "%s is not MY_NULLPTR", key.name);
        assert_true(key.value != MY_NULLPTR, msg);
        if (key.value != MY_NULLPTR) {
            snprintf(msg, sizeof(msg), "%s value '%s' is unique", key.name, key.value);
            assert_true(keyValues.find(key.value) == keyValues.end(), msg);
            keyValues.insert(key.value);
        }
    }

    if (gTestFailures > 0) {
        printf("\nFinished running tests: %d failures\n", gTestFailures);
        return 1;
    }

    printf("\nAll tests passed!\n");
    return 0;
}
