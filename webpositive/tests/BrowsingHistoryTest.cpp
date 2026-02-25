#include <stdio.h>
#include <DateTime.h>
#include <Message.h>
#include <String.h>
#include "BrowsingHistory.h"

const char* kApplicationName = "WebPositive";

int gTestFailures = 0;

void assert_int_equal(int32 expected, int32 actual, const char* message) {
    if (expected == actual) {
        printf("PASS: %s (%d == %d)\n", message, (int)expected, (int)actual);
    } else {
        printf("FAIL: %s (expected %d, but got %d)\n", message, (int)expected, (int)actual);
        gTestFailures++;
    }
}

void assert_uint32_equal(uint32 expected, uint32 actual, const char* message) {
    if (expected == actual) {
        printf("PASS: %s (%u == %u)\n", message, expected, actual);
    } else {
        printf("FAIL: %s (expected %u, but got %u)\n", message, expected, actual);
        gTestFailures++;
    }
}

void assert_true(bool condition, const char* message) {
    if (condition) {
        printf("PASS: %s\n", message);
    } else {
        printf("FAIL: %s\n", message);
        gTestFailures++;
    }
}

class BrowsingHistoryTest {
public:
    static void Run() {
        BrowsingHistoryTest tester;
        tester.test_insertion_index_empty();
        tester.test_insertion_index_order();
        tester.test_insertion_index_same_time();
        tester.test_insertion_index_duplicates();
        tester.test_add_item_new();
        tester.test_add_item_existing();
    }

private:
    void test_insertion_index_empty() {
        printf("Testing _InsertionIndex with empty history...\n");
        BrowsingHistory history(false);
        BrowsingHistoryItem item(BString("http://a.com"));
        assert_int_equal(0, history._InsertionIndex(&item), "Index in empty history should be 0");
    }

    void test_insertion_index_order() {
        printf("Testing _InsertionIndex chronological order...\n");
        BrowsingHistory history(false);

        BDateTime dt = BDateTime::CurrentDateTime(B_LOCAL_TIME);

        BDateTime dt_old = dt;
        dt_old.Date().AddDays(-1);

        BDateTime dt_new = dt;
        dt_new.Date().AddDays(1);

        BrowsingHistoryItem item_old(BString("http://old.com"), dt_old);
        BrowsingHistoryItem item_mid(BString("http://mid.com"), dt);
        BrowsingHistoryItem item_new(BString("http://new.com"), dt_new);

        history._AddItem(item_old, true);
        history._AddItem(item_new, true);

        // Check where mid goes. Should be between old and new.
        assert_int_equal(1, history._InsertionIndex(&item_mid), "Middle time item should go to index 1");

        // Check where a very old item goes (before old)
        BDateTime dt_very_old = dt_old;
        dt_very_old.Date().AddDays(-1);
        BrowsingHistoryItem item_very_old(BString("http://veryold.com"), dt_very_old);
        assert_int_equal(0, history._InsertionIndex(&item_very_old), "Very old item should go to index 0");

        // Check where a very new item goes (after new)
        BDateTime dt_very_new = dt_new;
        dt_very_new.Date().AddDays(1);
        BrowsingHistoryItem item_very_new(BString("http://verynew.com"), dt_very_new);
        assert_int_equal(2, history._InsertionIndex(&item_very_new), "Very new item should go to index 2");
    }

    void test_insertion_index_same_time() {
        printf("Testing _InsertionIndex with same time, different URLs...\n");
        BrowsingHistory history(false);

        BDateTime dt = BDateTime::CurrentDateTime(B_LOCAL_TIME);

        BrowsingHistoryItem itemA(BString("http://a.com"), dt);
        BrowsingHistoryItem itemC(BString("http://c.com"), dt);
        BrowsingHistoryItem itemB(BString("http://b.com"), dt);

        history._AddItem(itemA, true);
        history._AddItem(itemC, true);

        assert_int_equal(1, history._InsertionIndex(&itemB), "URL 'b' should go between 'a' and 'c' if times are equal");
    }

    void test_insertion_index_duplicates() {
        printf("Testing _InsertionIndex with duplicates...\n");
        BrowsingHistory history(false);

        BDateTime dt = BDateTime::CurrentDateTime(B_LOCAL_TIME);
        BrowsingHistoryItem item1(BString("http://test.com"), dt);
        history._AddItem(item1, true);

        BrowsingHistoryItem item1_dup(BString("http://test.com"), dt);
        // Should go at index 0 because it's equal to existing item1 and we use lower_bound
        assert_int_equal(0, history._InsertionIndex(&item1_dup), "Duplicate item should go at index 0");
    }

    void test_add_item_new() {
        printf("Testing AddItem with new URL...\n");
        BrowsingHistory history(false);
        assert_int_equal(0, history.CountItems(), "History should be empty initially");

        BrowsingHistoryItem item(BString("http://new.com"));
        bool added = history.AddItem(item);
        assert_true(added, "AddItem should return true");
        assert_int_equal(1, history.CountItems(), "History should have 1 item");
        assert_true(history.HistoryItemAt(0).URL() == "http://new.com", "Item URL should match");
    }

    void test_add_item_existing() {
        printf("Testing AddItem with existing URL...\n");
        BrowsingHistory history(false);

        BrowsingHistoryItem item1(BString("http://test.com"));
        history.AddItem(item1);
        int32 initialCount = history.CountItems();
        assert_int_equal(1, initialCount, "History should have 1 item");

        uint32 initialInvokations = history.HistoryItemAt(0).InvokationCount();

        // Add same URL again
        BrowsingHistoryItem item2(BString("http://test.com"));
        bool added = history.AddItem(item2);

        assert_true(added, "AddItem should return true for existing item");
        assert_int_equal(initialCount, history.CountItems(), "Count should NOT increase for existing URL");

        uint32 newInvokations = history.HistoryItemAt(0).InvokationCount();
        assert_uint32_equal(initialInvokations + 1, newInvokations, "Invokation count should increase");
    }
};

int main() {
    BrowsingHistoryTest::Run();

    if (gTestFailures > 0) {
        printf("\nFinished running tests: %d failures\n", gTestFailures);
        return 1;
    }

    printf("\nAll tests passed!\n");
    return 0;
}
