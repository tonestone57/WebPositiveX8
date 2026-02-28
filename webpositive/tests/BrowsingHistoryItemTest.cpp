#include "BeOSCompatibility.h"
#include <stdio.h>
#include <DateTime.h>
#include <Message.h>
#include <String.h>
#include "BrowsingHistory.h"

const char* kApplicationName = "WebPositive";

int gTestFailures =  0;

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
    assert_true(item.InvocationCount() == 0, "Initial invocation count is 0");

    BDateTime before = item.DateTime();
    assert_true(before.IsValid(), "Initial DateTime is valid");

    item.Invoked();
    assert_true(item.InvocationCount() == 1, "Invocation count is 1 after one call");
    assert_true(item.DateTime() >= before, "DateTime is updated (>= before)");
    assert_true(item.DateTime().IsValid(), "DateTime after invocation is valid");

    item.Invoked();
    assert_true(item.InvocationCount() == 2, "Invocation count is 2 after two calls");

    // Test overflow protection
    printf("Testing overflow protection...\n");
    BMessage archive;
    archive.AddString("url", "http://www.haiku-os.org");
    // Test that new key "invocations" works
    uint32 maxCount = 0xFFFFFFFF;
    archive.AddUInt32("invocations", maxCount);

    BrowsingHistoryItem overflowItem(&archive);
    assert_true(overflowItem.InvocationCount() == maxCount, "Item initialized with max uint32");

    overflowItem.Invoked();
    assert_true(overflowItem.InvocationCount() == maxCount, "Invocation count stays at max uint32 on overflow (clamping)");

    // Test backward compatibility with old key "invokations"
    printf("Testing backward compatibility with 'invokations' key...\n");
    BMessage oldArchive;
    oldArchive.AddString("url", "http://www.haiku-os.org");
    oldArchive.AddUInt32("invokations", 42);
    BrowsingHistoryItem oldItem(&oldArchive);
    assert_true(oldItem.InvocationCount() == 42, "Old 'invokations' key should be supported");
}

int main() {
    test_invoked();

    printf("\nTesting BrowsingHistoryItem assignment operator...\n");

    BrowsingHistoryItem item1(BString("http://www.google.com"));
    item1.Invoked();
    item1.Invoked();

    BrowsingHistoryItem item2(BString("http://www.haiku-os.org"));

    assert_true(item1.URL() == "http://www.google.com", "item1 URL is correct");
    assert_true(item1.InvocationCount() == 2, "item1 invocation count is 2");
    assert_true(item2.URL() == "http://www.haiku-os.org", "item2 URL is correct");
    assert_true(item2.InvocationCount() == 0, "item2 invocation count is 0");

    printf("Assigning item1 to item2...\n");
    item2 = item1;

    assert_true(item2.URL() == item1.URL(), "item2 URL matches item1 URL after assignment");
    assert_true(item2.DateTime() == item1.DateTime(), "item2 DateTime matches item1 DateTime after assignment");
    assert_true(item2.InvocationCount() == item1.InvocationCount(), "item2 invocation count matches item1 after assignment");
    assert_true(item2 == item1, "item2 == item1 after assignment");

    printf("Testing self-assignment...\n");
    item1 = item1;
    assert_true(item1.InvocationCount() == 2, "item1 invocation count still correct after self-assignment");

    printf("Testing multiple assignment...\n");
    BrowsingHistoryItem item3(BString("http://www.example.com"));
    item3 = item2 = item1;
    assert_true(item3 == item1, "item3 == item1 after multiple assignment");
    assert_true(item2 == item1, "item2 == item1 after multiple assignment");

    printf("\nTesting BrowsingHistoryItem comparison operators...\n");
    BrowsingHistoryItem itemA(BString("http://a.com"));
    BrowsingHistoryItem itemB(BString("http://b.com"));

    // If they have same time, URL should decide
    assert_true(itemA < itemB, "a.com < b.com");
    assert_true(itemB > itemA, "b.com > a.com");
    assert_true(itemA != itemB, "a.com != b.com");
    assert_true(itemA == itemA, "a.com == a.com");

    // Test with different times
    printf("Testing with different times...\n");
    BDateTime dt_old = itemA.DateTime();
    BDateTime dt_new = dt_old;
    dt_new.Date().AddDays(1);

    BrowsingHistoryItem itemA_newer(BString("http://a.com"), dt_new);
    assert_true(itemA < itemA_newer, "older a.com < newer a.com");
    assert_true(itemA_newer > itemA, "newer a.com > older a.com");

    // Newer item with earlier URL vs older item with later URL
    // Time should take precedence
    BrowsingHistoryItem item0_newer(BString("http://0.com"), dt_new);
    assert_true(itemB < item0_newer, "older b.com < newer 0.com (time takes precedence)");

    if (gTestFailures > 0) {
        printf("\nFinished running tests: %d failures\n", gTestFailures);
        return 1;
    }

    printf("\nAll tests passed!\n");
    return 0;
}
