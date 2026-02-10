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

### 3. `webpositive/autocompletion/AutoCompleter.h`

**Class:** `BAutoCompleter::CompletionStyle` (Pure C++ Class)

*   **Members:**
    *   `EditView* fEditView;`
    *   `PatternSelector* fPatternSelector;`
    *   `ChoiceModel* fChoiceModel;`
    *   `ChoiceView* fChoiceView;`
    *   **Reason:** These members are pointers to abstract base classes (interfaces) that are owned by `CompletionStyle`. The destructor manually deletes them.
    *   **Recommendation:** Change to `std::unique_ptr<EditView>`, `std::unique_ptr<PatternSelector>`, `std::unique_ptr<ChoiceModel>`, and `std::unique_ptr<ChoiceView>`. Update setters to take `std::unique_ptr` to enforce ownership transfer.

**Class:** `BAutoCompleter` (Pure C++ Class)

*   **Member:** `CompletionStyle* fCompletionStyle;`
    *   **Reason:** This member owns the `CompletionStyle` strategy object. The destructor manually deletes it.
    *   **Recommendation:** Change to `std::unique_ptr<CompletionStyle> fCompletionStyle;`.

### 4. `webpositive/tabview/TabManager.h`

**Class:** `TabManager` (Pure C++ Class)

*   **Member:** `TabManagerController* fController;`
    *   **Reason:** `TabManager` owns the `TabManagerController` instance (allocated in constructor, deleted in destructor). `TabManagerController` is a helper class implementing a pure virtual interface (`TabContainerView::Controller`).
    *   **Recommendation:** Change to `std::unique_ptr<TabManagerController> fController;`. Note: Ensure `TabManagerController` definition is available to `unique_ptr` destructor (in `.cpp` file).

## Task List

- [ ] Refactor `BrowsingHistory::fPendingSaveItems` to use `std::unique_ptr`.
- [ ] Refactor `CredentialsStorage::fPendingSaveMessage` to use `std::unique_ptr`.
- [ ] Refactor `BAutoCompleter::CompletionStyle` members to use `std::unique_ptr` and update setter signatures.
- [ ] Refactor `BAutoCompleter::fCompletionStyle` to use `std::unique_ptr`.
- [ ] Refactor `TabManager::fController` to use `std::unique_ptr`.
