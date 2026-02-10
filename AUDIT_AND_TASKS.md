# Code Audit and Task List

## Summary

This document summarizes the findings from a code audit of the WebPositive application, focusing on performance, stability, and threading.

## Architecture Overview

WebPositive is a C++ application built on the Haiku API. It uses the `BApplication` class for the main event loop and `BWindow` for UI windows. Each `BWindow` runs in its own thread (Looper).

The application uses Haiku WebKit (`BWebView`) for rendering web content. WebKit handles network requests and rendering in its own threads/processes.

## Performance Analysis

### Threading Model

The application relies on the standard Haiku threading model. Most UI operations happen on the window thread. Heavy computations or blocking I/O on the window thread will cause UI freezes.

### Synchronous I/O Bottlenecks

The primary performance bottleneck identified is synchronous file I/O on the main/window thread. This occurs in several places:

1.  **Browsing History (`BrowsingHistory.cpp`)**:
    *   **Issue**: `_SaveSettings` is called synchronously every time a page finishes loading (via `UpdateGlobalHistory` -> `AddItem`).
    *   **Impact**: Writing the entire history to disk on every page load causes noticeable UI stutter, especially as the history grows large.
    *   **Severity**: High.

2.  **Download Settings (`DownloadWindow.cpp`)**:
    *   **Issue**: `_SaveSettings` is called synchronously when downloads start, finish, or are removed.
    *   **Impact**: UI freezes during download operations.
    *   **Severity**: Medium.

3.  **Cookies (`BrowserApp.cpp`, `CookieWindow.cpp`)**:
    *   **Issue**: Cookies are loaded synchronously on startup and saved on exit.
    *   **Impact**: Slow startup and shutdown times if the cookie jar is large.
    *   **Severity**: Low to Medium.

4.  **Credentials (`CredentialsStorage.cpp`)**:
    *   **Issue**: Credentials are loaded lazily but synchronously. Saved on destruction.
    *   **Impact**: Minor impact unless many credentials are stored.
    *   **Severity**: Low.

## Stability Analysis

*   **Race Conditions**: The use of singletons (`BrowsingHistory`, `CredentialsStorage`) requires careful locking. The current implementation uses `BLocker`, which is correct for thread safety but contributes to lock contention and UI blocking if held during I/O.
*   **Crash Recovery**: If the application crashes, unsaved data (e.g., history since the last save) might be lost. However, the current aggressive saving strategy minimizes data loss at the cost of performance.

## Task List for Improvements

### High Priority

1.  **Refactor `BrowsingHistory` to use Asynchronous Saving**:
    *   **Goal**: Move file writing to a background thread to prevent UI blocking.
    *   **Task**: Implement a worker thread in `BrowsingHistory` that consumes save requests. Copy history items before passing them to the worker thread to ensure thread safety without blocking the main thread for I/O.
    *   **Status**: Completed.

### Medium Priority

2.  **Refactor `DownloadWindow` Settings Saving**:
    *   **Goal**: Prevent UI freezes during download management.
    *   **Task**: Implement a similar background saving mechanism or throttle the saving frequency (e.g., using `BMessageRunner`).
    *   **Status**: Completed.

3.  **Optimize Application Startup**:
    *   **Goal**: Improve launch time.
    *   **Task**: Load history and cookies asynchronously. Display the UI immediately and populate history/cookies when loaded.
    *   **Status**: Completed.

### Low Priority

4.  **Refactor `CredentialsStorage`**:
    *   **Goal**: Ensure non-blocking access.
    *   **Task**: Use async I/O for saving credentials.
    *   **Status**: Completed.

5.  **Code Cleanup**:
    *   **Goal**: Modernize C++ usage.
    *   **Task**: Use smart pointers and standard containers where appropriate, while maintaining compatibility with Haiku API.
    *   **Status**: Ongoing (Analysis Complete).
