#include <stdio.h>
#include <DateTime.h>
#include <Message.h>
#include <String.h>
#include "BrowsingHistory.h"

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

void test_invoked() {
    printf("Testing BrowsingHistoryItem::Invoked()...\n");
    BrowsingHistoryItem item(BString("http://www.haiku-os.org"));
    assert_true(item.InvokationCount() == 0, "Initial invokation count is 0");

    BDateTime before = item.DateTime();

    item.Invoked();
    assert_true(item.InvokationCount() == 1, "Invokation count is 1 after one call");
    assert_true(item.DateTime() >= before, "DateTime is updated (>= before)");

    item.Invoked();
    assert_true(item.InvokationCount() == 2, "Invokation count is 2 after two calls");

    // Test overflow
    printf("Testing overflow protection...\n");
    BMessage archive;
    archive.AddString("url", "http://www.haiku-os.org");
    archive.AddUInt32("invokations", 0xFFFFFFFF);

    BrowsingHistoryItem overflowItem(&archive);
    assert_true(overflowItem.InvokationCount() == 0xFFFFFFFF, "Item initialized with max uint32");

    overflowItem.Invoked();
    assert_true(overflowItem.InvokationCount() == 0xFFFFFFFF, "Invokation count stays at max uint32 on overflow");
}

int main() {
    test_invoked();

    printf("\nTesting BrowsingHistoryItem assignment operator...\n");

    BrowsingHistoryItem item1(BString("http://www.google.com"));
    // Simulate some state
    item1.Invoked();
    item1.Invoked();

    BrowsingHistoryItem item2(BString("http://www.haiku-os.org"));

    printf("Initial state:\n");
    printf("Item 1 URL: %s, Count: %u\n", item1.URL().String(), item1.InvokationCount());
    printf("Item 2 URL: %s, Count: %u\n", item2.URL().String(), item2.InvokationCount());

    assert_true(item1.URL() == "http://www.google.com", "item1 URL is correct");
    assert_true(item1.InvokationCount() == 2, "item1 invokation count is 2");
    assert_true(item2.URL() == "http://www.haiku-os.org", "item2 URL is correct");
    assert_true(item2.InvokationCount() == 0, "item2 invokation count is 0");

    printf("Assigning item1 to item2...\n");
    item2 = item1;

    assert_true(item2.URL() == item1.URL(), "item2 URL matches item1 URL after assignment");
    assert_true(item2.DateTime() == item1.DateTime(), "item2 DateTime matches item1 DateTime after assignment");
    assert_true(item2.InvokationCount() == item1.InvokationCount(), "item2 invokation count matches item1 after assignment");
    assert_true(item2 == item1, "item2 == item1 after assignment");

    printf("Testing self-assignment...\n");
    item1 = item1;
    assert_true(item1.URL() == "http://www.google.com", "item1 URL still correct after self-assignment");
    assert_true(item1.InvokationCount() == 2, "item1 invokation count still correct after self-assignment");

    printf("Testing multiple assignment...\n");
    BrowsingHistoryItem item3(BString("http://www.example.com"));
    item3 = item2 = item1;
    assert_true(item3 == item1, "item3 == item1 after multiple assignment");
    assert_true(item2 == item1, "item2 == item1 after multiple assignment");

    if (gTestFailures > 0) {
        printf("\nFinished running tests: %d failures\n", gTestFailures);
        return 1;
    }

    printf("\nAll tests passed!\n");
    return 0;
}
