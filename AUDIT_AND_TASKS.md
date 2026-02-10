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
    *   **Status**: Completed (Refactored to use BObjectList).
# Smart Pointer Audit for Pure C++ Classes

## Overview
This document summarizes the findings of a code audit to identify opportunities for using `std::unique_ptr` in "Pure C++ Classes" within the WebPositive codebase. The focus is on classes that do not inherit from Haiku API classes like `BArchivable`, `BHandler`, `BLooper`, `BWindow`, or `BView`, where memory management is typically handled by the API (reference counting or ownership hierarchy).

## Identified Candidates

### 1. `webpositive/BrowsingHistory.h`

**Class:** `BrowsingHistory` (inherits `BLocker`)

*   **Member:** `BObjectList<BrowsingHistoryItem, true>* fPendingSaveItems;`
    *   **Reason:** This pointer owns a heap-allocated `BObjectList` which owns its items. It is manually deleted and assigned new instances during save operations.
    *   **Recommendation:** Change to `std::unique_ptr<BObjectList<BrowsingHistoryItem, true>> fPendingSaveItems;`. This will automate the deletion and clarify ownership transfer to the save thread.

### 2. `webpositive/CredentialsStorage.h`

**Class:** `CredentialsStorage` (inherits `BLocker`)

*   **Member:** `BMessage* fPendingSaveMessage;`
    *   **Reason:** This pointer owns a heap-allocated `BMessage` used to pass data to the save thread. `BMessage` is a value-semantic class often allocated on the heap, and does not inherit from `BArchivable` (it has archiving methods but is not part of the archiving hierarchy). It is manually deleted.
    *   **Recommendation:** Change to `std::unique_ptr<BMessage> fPendingSaveMessage;`.

### 3. `webpositive/tabview/TabManager.h`

**Class:** `TabManager` (Pure C++ Class)

*   **Member:** `TabManagerController* fController;`
    *   **Reason:** `TabManager` owns the `TabManagerController` instance (allocated in constructor, deleted in destructor). `TabManagerController` is a helper class implementing a pure virtual interface (`TabContainerView::Controller`).
    *   **Recommendation:** Change to `std::unique_ptr<TabManagerController> fController;`. Note: Ensure `TabManagerController` definition is available to `unique_ptr` destructor (in `.cpp` file).

## Excluded Candidates

### `webpositive/autocompletion/AutoCompleter.h`

*   **Class:** `BAutoCompleter::CompletionStyle`
*   **Member:** `ChoiceView* fChoiceView;` (and other strategy objects)
*   **Reason:** While `ChoiceView` is a pure C++ interface, its concrete implementation `BDefaultChoiceView` creates and manages a `BWindow`. The interaction between `BWindow` ownership (which deletes its children and itself on `Quit()`) and `std::unique_ptr` ownership of the `ChoiceView` (which owns the window via a raw pointer) is complex and potentially prone to double-free or dangling pointer issues if not handled with extreme care regarding Haiku's window lifecycle. It was decided to exclude this from the initial smart pointer refactoring to avoid regressions.

## Task List

- [x] Refactor `BrowsingHistory::fPendingSaveItems` to use `std::unique_ptr`.
- [x] Refactor `CredentialsStorage::fPendingSaveMessage` to use `std::unique_ptr`.
- [x] Refactor `TabManager::fController` to use `std::unique_ptr`.
