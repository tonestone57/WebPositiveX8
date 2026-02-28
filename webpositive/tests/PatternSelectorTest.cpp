#include "BeOSCompatibility.h"
#include <stdio.h>
#include "AutoCompleterDefaultImpl.h"

int gTestFailures =  0;

void assert_int32(int32 expected, int32 actual, const char* message) {
    if (actual == expected) {
        printf("PASS: %s\n", message);
    } else {
        printf("FAIL: %s (expected %d, got %d)\n", message, expected, actual);
        gTestFailures++;
    }
}

int main() {
    BDefaultPatternSelector selector;
    int32 start, length;

    printf("Testing BDefaultPatternSelector::SelectPatternBounds...\n");

    // Test case 1: Normal text
    BString text1("https://www.haiku-os.org");
    start = -1; length = -1;
    selector.SelectPatternBounds(text1, 5, &start, &length);
    assert_int32(0, start, "Start should be 0 for normal text");
    assert_int32(text1.Length(), length, "Length should be full text length for normal text");

    // Test case 2: Empty text
    BString text2("");
    start = -1; length = -1;
    selector.SelectPatternBounds(text2, 0, &start, &length);
    assert_int32(0, start, "Start should be 0 for empty text");
    assert_int32(0, length, "Length should be 0 for empty text");

    // Test case 3: Caret position in the middle
    BString text3("Search query");
    start = -1; length = -1;
    selector.SelectPatternBounds(text3, 6, &start, &length);
    assert_int32(0, start, "Start should be 0 regardless of caret position");
    assert_int32(text3.Length(), length, "Length should be full text length regardless of caret position");

    // Test case 4: MY_NULLPTR pointers (should not crash)
    printf("Testing MY_NULLPTR pointers (should not crash)...\n");
    selector.SelectPatternBounds(text1, 0, 0, &length);
    selector.SelectPatternBounds(text1, 0, &start, 0);
    selector.SelectPatternBounds(text1, 0, 0, 0);
    printf("PASS: MY_NULLPTR pointers handled without crash\n");

    if (gTestFailures > 0) {
        printf("\nFinished running tests: %d failures\n", gTestFailures);
        return 1;
    }

    printf("\nAll tests passed!\n");
    return 0;
}
