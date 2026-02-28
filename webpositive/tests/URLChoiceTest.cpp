#include "BeOSCompatibility.h"
#include <stdio.h>
#include <stdlib.h>
#include "URLChoice.h"

int gTestFailures = 0;

void assert_bool(bool expected, bool actual, const char* message) {
    if (actual == expected) {
        printf("PASS: %s\n", message);
    } else {
        printf("FAIL: %s (expected %s, got %s)\n", message, expected ? "true" : "false", actual ? "true" : "false");
        gTestFailures++;
    }
}

int main() {
    URLChoice a("http://a.com", "a.com", 0, 0, 10);
    URLChoice b("http://b.com", "b.com", 0, 0, 20);

    printf("Testing URLChoice comparison logic...\n");

    // Case 1: Higher priority should come first (return true for <)
    assert_bool(true, b < a, "Higher priority (20) < lower priority (10) should be true");
    assert_bool(false, a < b, "Lower priority (10) < higher priority (20) should be false");

    // Case 2: Lower priority with smaller text
    URLChoice c("http://aaa.com", "aaa.com", 0, 0, 5);
    URLChoice d("http://zzz.com", "zzz.com", 0, 0, 20);
    // d has higher priority, so d < c should be true.
    assert_bool(true, d < c, "Higher priority with larger text < lower priority with smaller text should be true");
    assert_bool(false, c < d, "Lower priority with smaller text < higher priority with larger text should be false");

    // Case 3: Same priority, different text
    URLChoice e("http://e.com", "e.com", 0, 0, 10);
    URLChoice f("http://f.com", "f.com", 0, 0, 10);
    assert_bool(true, e < f, "Same priority, smaller text < larger text should be true");
    assert_bool(false, f < e, "Same priority, larger text < smaller text should be false");

    // Case 4: Identity (operator==)
    assert_bool(true, a == a, "a == a should be true");

    // Case 5: Equality with different priorities but same text
    URLChoice a2("http://a.com", "a.com", 0, 0, 20);
    assert_bool(false, a == a2, "a == a2 should be false (different priority)");

    // Case 6: Equality with same priority but different text
    assert_bool(false, e == f, "e == f should be false (different text)");

    if (gTestFailures > 0) {
        printf("Finished running tests: %d failures\n", gTestFailures);
        return 1;
    }

    printf("All tests passed!\n");
    return 0;
}
