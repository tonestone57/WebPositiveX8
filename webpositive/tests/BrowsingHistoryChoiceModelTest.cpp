#include "BeOSCompatibility.h"
#include <stdio.h>
#include <limits.h>
#include <DateTime.h>
#include <String.h>
#include "BrowsingHistoryChoiceModel.h"
#include "BrowsingHistory.h"
#include "URLChoice.h"

const char* kApplicationName = "WebPositive";

int gTestFailures = 0;

void assert_true(bool condition, const char* message) {
    if (condition) {
        printf("PASS: %s\n", message);
    } else {
        printf("FAIL: %s\n", message);
        gTestFailures++;
    }
}

void assert_int_equal(int32 expected, int32 actual, const char* message) {
    if (expected == actual) {
        printf("PASS: %s (%d == %d)\n", message, (int)expected, (int)actual);
    } else {
        printf("FAIL: %s (expected %d, but got %d)\n", message, (int)expected, (int)actual);
        gTestFailures++;
    }
}

class MockBrowsingHistory : public BrowsingHistory {
public:
    MockBrowsingHistory() : BrowsingHistory(false) {}

    virtual int32 CountItems() const {
        return fItems.CountItems();
    }

    virtual const BrowsingHistoryItem* ItemAt(int32 index) const {
        return fItems.ItemAt(index);
    }

    void AddTestItem(const char* url) {
        fItems.AddItem(new BrowsingHistoryItem(BString(url)));
    }

private:
    BObjectList<BrowsingHistoryItem, true> fItems;
};

void test_fetch_choices_empty() {
    printf("Testing FetchChoicesFor with empty history...\n");
    MockBrowsingHistory history;
    BrowsingHistoryChoiceModel model(&history);

    model.FetchChoicesFor(BString("google"));
    assert_int_equal(0, model.CountChoices(), "No choices for empty history");
}

void test_fetch_choices_no_match() {
    printf("Testing FetchChoicesFor with no matches...\n");
    MockBrowsingHistory history;
    history.AddTestItem("http://www.haiku-os.org");
    history.AddTestItem("http://www.google.com");

    BrowsingHistoryChoiceModel model(&history);

    model.FetchChoicesFor(BString("yahoo"));
    assert_int_equal(0, model.CountChoices(), "No choices when no match");
}

void test_fetch_choices_single_match() {
    printf("Testing FetchChoicesFor with single match...\n");
    MockBrowsingHistory history;
    history.AddTestItem("http://www.haiku-os.org");
    history.AddTestItem("http://www.google.com");

    BrowsingHistoryChoiceModel model(&history);

    model.FetchChoicesFor(BString("google"));
    assert_int_equal(1, model.CountChoices(), "One choice for single match");
    assert_true(model.ChoiceAt(0)->Text() == "http://www.google.com", "Correct choice text");
}

void test_fetch_choices_multiple_matches() {
    printf("Testing FetchChoicesFor with multiple matches...\n");
    MockBrowsingHistory history;
    history.AddTestItem("http://www.haiku-os.org");
    history.AddTestItem("http://www.google.com");
    history.AddTestItem("https://www.google.com/search?q=haiku");

    BrowsingHistoryChoiceModel model(&history);

    model.FetchChoicesFor(BString("google"));
    assert_int_equal(2, model.CountChoices(), "Two choices for multiple matches");
}

void test_fetch_choices_priority() {
    printf("Testing FetchChoicesFor priority logic...\n");
    MockBrowsingHistory history;
    // Items are added in order. In the model, if consecutive items have the same base URL, priority decreases.
    // Wait, history items in real BrowsingHistory are sorted by date.
    // Our MockBrowsingHistory keeps them in the order we added them.

    history.AddTestItem("http://www.google.com/1");
    history.AddTestItem("http://www.google.com/2");
    history.AddTestItem("http://www.haiku-os.org");

    BrowsingHistoryChoiceModel model(&history);

    model.FetchChoicesFor(BString("http"));
    assert_int_equal(3, model.CountChoices(), "Three choices");

    // http://www.google.com/1 -> priority INT_MAX
    // http://www.google.com/2 -> priority INT_MAX - 1 (same base URL)
    // http://www.haiku-os.org -> priority INT_MAX (different base URL)

    // Sorted by priority (descending) and then display text (ascending).
    // So 1st should be http://www.google.com/1 (priority MAX) or http://www.haiku-os.org (priority MAX)
    // Between those two, "http://www.google.com/1" < "http://www.haiku-os.org" alphabetically? No, 'g' < 'h'.

    assert_true(model.ChoiceAt(0)->Text() == "http://www.google.com/1", "First choice is google 1");
    assert_true(model.ChoiceAt(1)->Text() == "http://www.haiku-os.org", "Second choice is haiku-os");
    assert_true(model.ChoiceAt(2)->Text() == "http://www.google.com/2", "Third choice is google 2");
}

int main() {
    test_fetch_choices_empty();
    test_fetch_choices_no_match();
    test_fetch_choices_single_match();
    test_fetch_choices_multiple_matches();
    test_fetch_choices_priority();

    if (gTestFailures > 0) {
        printf("\nFinished running tests: %d failures\n", gTestFailures);
        return 1;
    }

    printf("\nAll tests passed!\n");
    return 0;
}
