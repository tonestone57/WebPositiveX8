/*
 * Copyright (C) 2024 Haiku, Inc.
 *
 * All rights reserved. Distributed under the terms of the MIT License.
 */


#include "BeOSCompatibility.h"
#include <stdio.h>
#include <SupportDefs.h>
#include "DownloadSpeedCalculator.h"


int gTestFailures =  0;


void assert_true(bool condition, const char* message) {
    if (condition) {
        printf("PASS: %s\n", message);
    } else {
        printf("FAIL: %s\n", message);
        gTestFailures++;
    }
}


void assert_double_equals(double expected, double actual, const char* message, double epsilon = 0.001) {
    if (actual >= expected - epsilon && actual <= expected + epsilon) {
        printf("PASS: %s (expected: %f, actual: %f)\n", message, expected, actual);
    } else {
        printf("FAIL: %s (expected: %f, actual: %f)\n", message, expected, actual);
        gTestFailures++;
    }
}


void test_initial_state() {
    printf("Testing initial state...\n");
    DownloadSpeedCalculator calculator;
    assert_double_equals(0.0, calculator.CurrentSpeed(), "Initial speed should be 0.0");
}


void test_constant_speed() {
    printf("Testing constant speed...\n");
    DownloadSpeedCalculator calculator;
    bigtime_t now = 1000000; // 1 second
    calculator.Reset(0, now);

    // After 0.5s, update with 500 bytes (1000 bytes/s)
    now += 500000;
    calculator.Update(500, now);
    assert_double_equals(1000.0, calculator.CurrentSpeed(), "Speed after first update (0.5s, 500 bytes)");

    // After another 0.5s, update with another 500 bytes
    now += 500000;
    calculator.Update(1000, now);
    assert_double_equals(1000.0, calculator.CurrentSpeed(), "Speed after second update (1.0s total, 1000 bytes total)");

    assert_double_equals(1000.0, calculator.AverageSpeed(1000, now), "Average speed should be 1000.0");
}


void test_sliding_window() {
    printf("Testing sliding window...\n");
    DownloadSpeedCalculator calculator;
    bigtime_t now = 1000000;
    calculator.Reset(0, now);

    // Fill the window with 1000 bytes/s for 10 slots (5 seconds)
    for (int i = 1; i <= 10; i++) {
        now += 500000;
        calculator.Update(i * 500, now);
    }
    assert_double_equals(1000.0, calculator.CurrentSpeed(), "Speed after 10 updates of 1000 bytes/s");

    // Now change to 2000 bytes/s
    for (int i = 1; i <= 10; i++) {
        now += 500000;
        calculator.Update(5000 + i * 1000, now);
        // Speed should gradually increase
    }
    assert_double_equals(2000.0, calculator.CurrentSpeed(), "Speed after another 10 updates of 2000 bytes/s");
}


void test_average_speed() {
    printf("Testing average speed...\n");
    DownloadSpeedCalculator calculator;
    bigtime_t startTime = 1000000;
    calculator.Reset(0, startTime);

    bigtime_t now = startTime + 2000000; // 2 seconds later
    assert_double_equals(500.0, calculator.AverageSpeed(1000, now), "Average speed after 2s and 1000 bytes");

    now = startTime + 4000000; // 4 seconds later
    assert_double_equals(250.0, calculator.AverageSpeed(1000, now), "Average speed after 4s and 1000 bytes");
}


void test_update_throttling() {
    printf("Testing update throttling...\n");
    DownloadSpeedCalculator calculator;
    bigtime_t now = 1000000;
    calculator.Reset(0, now);

    // Update with 1000 bytes/s
    now += 500000;
    calculator.Update(500, now);
    assert_double_equals(1000.0, calculator.CurrentSpeed(), "Speed after valid update interval");

    // Try update too soon (0.1s later)
    now += 100000;
    calculator.Update(1000, now);
    assert_double_equals(1000.0, calculator.CurrentSpeed(), "Speed should NOT update if interval < kSpeedReferenceInterval");

    // Update after enough time (0.4s later, total 0.5s from last reference)
    now += 400000;
    calculator.Update(1500, now);
    // (1000 + 2000) / 2 = 1500
    assert_double_equals(1500.0, calculator.CurrentSpeed(), "Speed should update after total 0.5s from last reference");
}


int main() {
    test_initial_state();
    test_constant_speed();
    test_sliding_window();
    test_average_speed();
    test_update_throttling();

    if (gTestFailures > 0) {
        printf("\nFinished running tests: %d failures\n", gTestFailures);
        return 1;
    }

    printf("\nAll tests passed!\n");
    return 0;
}
