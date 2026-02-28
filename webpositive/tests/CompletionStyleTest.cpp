#include "BeOSCompatibility.h"
#include <stdio.h>
#include <vector>
#include <string.h>
#include <memory>
#include "AutoCompleterDefaultImpl.h"

int gTestFailures = 0;

void assert_bool(bool expected, bool actual, const char* message) {
    if (actual == expected) {
        printf("PASS: %s\n", message);
    } else {
        printf("FAIL: %s (expected %s, got %s)\n", message,
            expected ? "true" : "false", actual ? "true" : "false");
        gTestFailures++;
    }
}

void assert_string_equal(const char* expected, const char* actual, const char* message) {
    if (strcmp(expected, actual) == 0) {
        printf("PASS: %s\n", message);
    } else {
        printf("FAIL: %s (expected '%s', got '%s')\n", message, expected, actual);
        gTestFailures++;
    }
}

void assert_int32(int32 expected, int32 actual, const char* message) {
    if (actual == expected) {
        printf("PASS: %s\n", message);
    } else {
        printf("FAIL: %s (expected %d, got %d)\n", message, (int)expected, (int)actual);
        gTestFailures++;
    }
}

class MockEditView : public BAutoCompleter::EditView {
public:
    MockEditView() : fCaretPos(0), fSetEditViewStateCalled(0), fLastSetCaretPos(0) {}
    virtual BRect GetAdjustmentFrame() { return BRect(); }
    virtual void GetEditViewState(BString& text, int32* caretPos) {
        text = fText;
        if (caretPos)
            *caretPos = fCaretPos;
    }
    virtual void SetEditViewState(const BString& text, int32 caretPos, int32 selectionLength = 0) {
        fText = text;
        fCaretPos = caretPos;
        fSetEditViewStateCalled++;
        fLastSetText = text;
        fLastSetCaretPos = caretPos;
    }

    BString fText;
    int32 fCaretPos;
    int32 fSetEditViewStateCalled;
    BString fLastSetText;
    int32 fLastSetCaretPos;
};

class MockChoiceModel : public BAutoCompleter::ChoiceModel {
public:
    MockChoiceModel() : fFetchChoicesForCalled(0) {}
    virtual ~MockChoiceModel() {
        for (std::vector<BAutoCompleter::Choice*>::iterator it = fChoices.begin();
                it != fChoices.end(); ++it) {
            delete *it;
        }
    }
    virtual void FetchChoicesFor(const BString& pattern) {
        fFetchChoicesForCalled++;
        fLastPattern = pattern;
    }
    virtual int32 CountChoices() const { return (int32)fChoices.size(); }
    virtual const BAutoCompleter::Choice* ChoiceAt(int32 index) const {
        if (index >= 0 && index < (int32)fChoices.size())
            return fChoices[index];
        return MY_NULLPTR;
    }

    void AddChoice(const char* text) {
        fChoices.push_back(new BAutoCompleter::Choice(text, text, 0, 0));
    }

    void ClearChoices() {
        for (std::vector<BAutoCompleter::Choice*>::iterator it = fChoices.begin();
                it != fChoices.end(); ++it) {
            delete *it;
        }
        fChoices.clear();
    }

    int32 fFetchChoicesForCalled;
    BString fLastPattern;

private:
    std::vector<BAutoCompleter::Choice*> fChoices;
};

class MockChoiceView : public BAutoCompleter::ChoiceView {
public:
    MockChoiceView() : fSelectedIndex(-1), fSelectChoiceAtCalled(0),
        fShowChoicesCalled(0), fHideChoicesCalled(0), fChoicesAreShown(false) {}
    virtual void SelectChoiceAt(int32 index) {
        fSelectedIndex = index;
        fSelectChoiceAtCalled++;
    }
    virtual void ShowChoices(BAutoCompleter::CompletionStyle* completer) {
        fShowChoicesCalled++;
        fChoicesAreShown = true;
    }
    virtual void HideChoices() {
        fHideChoicesCalled++;
        fChoicesAreShown = false;
    }
    virtual bool ChoicesAreShown() { return fChoicesAreShown; }
    virtual int32 CountVisibleChoices() const { return 0; }

    int32 fSelectedIndex;
    int32 fSelectChoiceAtCalled;
    int32 fShowChoicesCalled;
    int32 fHideChoicesCalled;
    bool fChoicesAreShown;
};

class MockPatternSelector : public BAutoCompleter::PatternSelector {
public:
    MockPatternSelector() : fStart(0), fLength(0) {}
    virtual void SelectPatternBounds(const BString& text, int32 caretPos, int32* start, int32* length) {
        if (start)
            *start = fStart;
        if (length)
            *length = fLength;
    }

    int32 fStart;
    int32 fLength;
};

void testSelectPrevious() {
    printf("Testing BDefaultCompletionStyle::SelectPrevious...\n");

    // CompletionStyle takes ownership and deletes these in its destructor
    MockEditView* editView = new MockEditView();
    MockChoiceModel* choiceModel = new MockChoiceModel();
    MockChoiceView* choiceView = new MockChoiceView();
    MockPatternSelector* patternSelector = new MockPatternSelector();

    BDefaultCompletionStyle style(editView, choiceModel, choiceView, patternSelector);

    // Test case 1: No choices
    assert_bool(false, style.SelectPrevious(), "SelectPrevious should return false when no choices");

    choiceModel->AddChoice("choice1");
    choiceModel->AddChoice("choice2");
    choiceModel->AddChoice("choice3");

    // Test case 2: Initial state (-1), wrap=false
    style.Select(-1);
    choiceView->fSelectChoiceAtCalled = 0;
    assert_bool(true, style.SelectPrevious(false), "SelectPrevious(false) from -1 should return true");
    assert_int32(0, style.SelectedChoiceIndex(), "SelectPrevious(false) from -1 should select 0");
    assert_int32(0, choiceView->fSelectedIndex, "ChoiceView should select 0");
    assert_int32(1, choiceView->fSelectChoiceAtCalled, "ChoiceView::SelectChoiceAt should be called once");

    // Test case 3: Initial state (-1), wrap=true
    style.Select(-1);
    choiceView->fSelectChoiceAtCalled = 0;
    assert_bool(true, style.SelectPrevious(true), "SelectPrevious(true) from -1 should return true");
    assert_int32(2, style.SelectedChoiceIndex(), "SelectPrevious(true) from -1 should select 2 (last item)");
    assert_int32(2, choiceView->fSelectedIndex, "ChoiceView should select 2");
    assert_int32(1, choiceView->fSelectChoiceAtCalled, "ChoiceView::SelectChoiceAt should be called once");

    // Test case 4: From middle (index 1)
    style.Select(1);
    choiceView->fSelectChoiceAtCalled = 0;
    assert_bool(true, style.SelectPrevious(false), "SelectPrevious from 1 should return true");
    assert_int32(0, style.SelectedChoiceIndex(), "SelectPrevious from 1 should select 0");
    assert_int32(0, choiceView->fSelectedIndex, "ChoiceView should select 0");

    // Test case 5: From 0, wrap=false
    style.Select(0);
    choiceView->fSelectChoiceAtCalled = 0;
    assert_bool(false, style.SelectPrevious(false), "SelectPrevious(false) from 0 should return false (already at 0)");
    assert_int32(0, style.SelectedChoiceIndex(), "SelectPrevious(false) from 0 should stay at 0");
    assert_int32(0, choiceView->fSelectChoiceAtCalled, "ChoiceView::SelectChoiceAt should NOT be called");

    // Test case 6: From 0, wrap=true
    style.Select(0);
    choiceView->fSelectChoiceAtCalled = 0;
    assert_bool(true, style.SelectPrevious(true), "SelectPrevious(true) from 0 should return true (wraps)");
    assert_int32(2, style.SelectedChoiceIndex(), "SelectPrevious(true) from 0 should wrap to 2");
    assert_int32(2, choiceView->fSelectedIndex, "ChoiceView should wrap to 2");
}

void testSelectNext() {
    printf("Testing BDefaultCompletionStyle::SelectNext...\n");

    // CompletionStyle takes ownership and deletes these in its destructor
    MockEditView* editView = new MockEditView();
    MockChoiceModel* choiceModel = new MockChoiceModel();
    MockChoiceView* choiceView = new MockChoiceView();
    MockPatternSelector* patternSelector = new MockPatternSelector();

    BDefaultCompletionStyle style(editView, choiceModel, choiceView, patternSelector);

    // Test case 1: No choices
    assert_bool(false, style.SelectNext(), "SelectNext should return false when no choices");

    choiceModel->AddChoice("choice1");
    choiceModel->AddChoice("choice2");
    choiceModel->AddChoice("choice3");

    // Test case 2: Initial state (-1), wrap=false
    style.Select(-1);
    choiceView->fSelectChoiceAtCalled = 0;
    assert_bool(true, style.SelectNext(false), "SelectNext(false) from -1 should return true");
    assert_int32(0, style.SelectedChoiceIndex(), "SelectNext(false) from -1 should select 0");
    assert_int32(0, choiceView->fSelectedIndex, "ChoiceView should select 0");
    assert_int32(1, choiceView->fSelectChoiceAtCalled, "ChoiceView::SelectChoiceAt should be called once");

    // Test case 3: Initial state (-1), wrap=true
    style.Select(-1);
    choiceView->fSelectChoiceAtCalled = 0;
    assert_bool(true, style.SelectNext(true), "SelectNext(true) from -1 should return true");
    assert_int32(0, style.SelectedChoiceIndex(), "SelectNext(true) from -1 should select 0");
    assert_int32(0, choiceView->fSelectedIndex, "ChoiceView should select 0");
    assert_int32(1, choiceView->fSelectChoiceAtCalled, "ChoiceView::SelectChoiceAt should be called once");

    // Test case 4: From middle (index 1)
    style.Select(1);
    choiceView->fSelectChoiceAtCalled = 0;
    assert_bool(true, style.SelectNext(false), "SelectNext from 1 should return true");
    assert_int32(2, style.SelectedChoiceIndex(), "SelectNext from 1 should select 2");
    assert_int32(2, choiceView->fSelectedIndex, "ChoiceView should select 2");

    // Test case 5: From last (index 2), wrap=false
    style.Select(2);
    choiceView->fSelectChoiceAtCalled = 0;
    assert_bool(false, style.SelectNext(false), "SelectNext(false) from 2 should return false (already at last)");
    assert_int32(2, style.SelectedChoiceIndex(), "SelectNext(false) from 2 should stay at 2");
    assert_int32(0, choiceView->fSelectChoiceAtCalled, "ChoiceView::SelectChoiceAt should NOT be called");

    // Test case 6: From last (index 2), wrap=true
    style.Select(2);
    choiceView->fSelectChoiceAtCalled = 0;
    assert_bool(true, style.SelectNext(true), "SelectNext(true) from 2 should return true (wraps)");
    assert_int32(0, style.SelectedChoiceIndex(), "SelectNext(true) from 2 should wrap to 0");
    assert_int32(0, choiceView->fSelectedIndex, "ChoiceView should wrap to 0");
}

void testSingleChoice() {
    printf("Testing BDefaultCompletionStyle with single choice...\n");

    // CompletionStyle takes ownership and deletes these in its destructor
    MockEditView* editView = new MockEditView();
    MockChoiceModel* choiceModel = new MockChoiceModel();
    MockChoiceView* choiceView = new MockChoiceView();
    MockPatternSelector* patternSelector = new MockPatternSelector();

    choiceModel->AddChoice("only");
    BDefaultCompletionStyle style(editView, choiceModel, choiceView, patternSelector);

    // SelectNext
    style.Select(-1);
    choiceView->fSelectChoiceAtCalled = 0;
    assert_bool(true, style.SelectNext(false), "SelectNext(false) from -1 with single choice should return true");
    assert_int32(0, style.SelectedChoiceIndex(), "SelectNext(false) from -1 with single choice should select 0");
    assert_int32(0, choiceView->fSelectedIndex, "ChoiceView should select 0");

    assert_bool(false, style.SelectNext(false), "SelectNext(false) from 0 with single choice should return false");
    assert_bool(false, style.SelectNext(true), "SelectNext(true) from 0 with single choice should return false");

    // SelectPrevious
    style.Select(-1);
    choiceView->fSelectChoiceAtCalled = 0;
    assert_bool(true, style.SelectPrevious(false), "SelectPrevious(false) from -1 with single choice should return true");
    assert_int32(0, style.SelectedChoiceIndex(), "SelectPrevious(false) from -1 with single choice should select 0");
    assert_int32(0, choiceView->fSelectedIndex, "ChoiceView should select 0");

    assert_bool(false, style.SelectPrevious(false), "SelectPrevious(false) from 0 with single choice should return false");
    assert_bool(false, style.SelectPrevious(true), "SelectPrevious(true) from 0 with single choice should return false");
}

void testEdgeCases() {
    printf("Testing BDefaultCompletionStyle edge cases...\n");

    // MY_NULLPTR ChoiceModel
    {
        // CompletionStyle takes ownership and deletes these in its destructor
        MockEditView* editView = new MockEditView();
        MockChoiceView* choiceView = new MockChoiceView();
        MockPatternSelector* patternSelector = new MockPatternSelector();
        BDefaultCompletionStyle style(editView, 0, choiceView, patternSelector);
        assert_bool(false, style.SelectNext(), "SelectNext should return false when ChoiceModel is MY_NULLPTR");
        assert_bool(false, style.SelectPrevious(), "SelectPrevious should return false when ChoiceModel is MY_NULLPTR");
    }

    // MY_NULLPTR ChoiceView
    {
        // CompletionStyle takes ownership and deletes these in its destructor
        MockEditView* editView = new MockEditView();
        MockChoiceModel* choiceModel = new MockChoiceModel();
        MockPatternSelector* patternSelector = new MockPatternSelector();
        BDefaultCompletionStyle style(editView, choiceModel, 0, patternSelector);
        choiceModel->AddChoice("choice1");
        assert_bool(false, style.SelectNext(), "SelectNext should return false when ChoiceView is MY_NULLPTR");
        assert_bool(false, style.SelectPrevious(), "SelectPrevious should return false when ChoiceView is MY_NULLPTR");
    }

    // Model changes (CountChoices changes)
    {
        // CompletionStyle takes ownership and deletes these in its destructor
        MockEditView* editView = new MockEditView();
        MockChoiceModel* choiceModel = new MockChoiceModel();
        MockChoiceView* choiceView = new MockChoiceView();
        MockPatternSelector* patternSelector = new MockPatternSelector();
        BDefaultCompletionStyle style(editView, choiceModel, choiceView, patternSelector);

        choiceModel->AddChoice("choice1");
        choiceModel->AddChoice("choice2");

        style.Select(1); // Select last
        assert_int32(1, style.SelectedChoiceIndex(), "Initially at index 1");

        // Model grows
        choiceModel->AddChoice("choice3");
        assert_bool(true, style.SelectNext(false), "SelectNext should return true now that model grew");
        assert_int32(2, style.SelectedChoiceIndex(), "Should select index 2");

        // Model shrinks below current selection
        choiceModel->ClearChoices();
        choiceModel->AddChoice("only");
        // style.fSelectedIndex is still 2, but CountChoices() is now 1.
        assert_bool(false, style.SelectNext(false), "SelectNext should return false when current index is out of bounds");
        assert_int32(2, style.SelectedChoiceIndex(), "Index should remain 2 (though invalid)");
    }

    // SetChoiceModel
    {
        // CompletionStyle takes ownership and deletes these in its destructor
        MockEditView* editView = new MockEditView();
        MockChoiceModel* choiceModel1 = new MockChoiceModel();
        MockChoiceView* choiceView = new MockChoiceView();
        MockPatternSelector* patternSelector = new MockPatternSelector();
        BDefaultCompletionStyle style(editView, choiceModel1, choiceView, patternSelector);

        choiceModel1->AddChoice("c1_1");
        style.Select(0);

        MockChoiceModel* choiceModel2 = new MockChoiceModel();
        choiceModel2->AddChoice("c2_1");
        choiceModel2->AddChoice("c2_2");

        // SetChoiceModel takes ownership of choiceModel2 and deletes choiceModel1
        style.SetChoiceModel(choiceModel2);
        // Note: style.fSelectedIndex is still 0.
        assert_int32(0, style.SelectedChoiceIndex(), "Index remains 0 after SetChoiceModel");

        assert_bool(true, style.SelectNext(false), "SelectNext works with new model");
        assert_int32(1, style.SelectedChoiceIndex(), "Should select index 1 of new model");
    }
}

void testApplyChoice() {
    printf("Testing BDefaultCompletionStyle::ApplyChoice...\n");

    MockEditView* editView = new MockEditView();
    MockChoiceModel* choiceModel = new MockChoiceModel();
    MockChoiceView* choiceView = new MockChoiceView();
    MockPatternSelector* patternSelector = new MockPatternSelector();

    BDefaultCompletionStyle style(editView, choiceModel, choiceView, patternSelector);

    choiceModel->AddChoice("apple");
    choiceModel->AddChoice("application");

    // Scenario: user typed "app", wants to complete to "application"
    editView->fText = "app";
    editView->fCaretPos = 3;
    patternSelector->fStart = 0;
    patternSelector->fLength = 3;

    style.EditViewStateChanged(true);
    style.Select(1); // select "application"

    choiceView->fHideChoicesCalled = 0;
    editView->fSetEditViewStateCalled = 0;

    style.ApplyChoice(true);

    assert_int32(1, editView->fSetEditViewStateCalled, "SetEditViewState should be called");
    assert_string_equal("application", editView->fLastSetText.String(), "Text should be completed to 'application'");
    assert_int32(11, editView->fLastSetCaretPos, "Caret should be at the end of completed text");
    assert_int32(1, choiceView->fHideChoicesCalled, "HideChoices should be called when hideChoices is true");
    assert_int32(-1, style.SelectedChoiceIndex(), "Selection should be reset when choices are hidden");

    // Scenario: user typed "test apple", wants to complete "apple" to "application"
    editView->fText = "test apple";
    editView->fCaretPos = 10;
    patternSelector->fStart = 5;
    patternSelector->fLength = 5;

    style.EditViewStateChanged(true);
    style.Select(1); // select "application"

    style.ApplyChoice(false); // don't hide

    assert_string_equal("test application", editView->fLastSetText.String(), "Partial completion should work");
    assert_int32(16, editView->fLastSetCaretPos, "Caret position should be correct after partial completion");
    assert_int32(1, choiceView->fHideChoicesCalled, "HideChoices should NOT be called again");
    assert_int32(1, style.SelectedChoiceIndex(), "Selection should remain when choices are NOT hidden");
}

void testCancelChoice() {
    printf("Testing BDefaultCompletionStyle::CancelChoice...\n");

    MockEditView* editView = new MockEditView();
    MockChoiceModel* choiceModel = new MockChoiceModel();
    MockChoiceView* choiceView = new MockChoiceView();
    MockPatternSelector* patternSelector = new MockPatternSelector();

    BDefaultCompletionStyle style(editView, choiceModel, choiceView, patternSelector);

    choiceModel->AddChoice("apple");

    editView->fText = "app";
    editView->fCaretPos = 3;
    patternSelector->fStart = 0;
    patternSelector->fLength = 3;

    style.EditViewStateChanged(true);
    choiceView->fChoicesAreShown = true;
    style.Select(0);

    editView->fSetEditViewStateCalled = 0;
    choiceView->fHideChoicesCalled = 0;

    style.CancelChoice();

    assert_int32(1, editView->fSetEditViewStateCalled, "SetEditViewState should be called to restore text");
    assert_string_equal("app", editView->fLastSetText.String(), "Original text should be restored");
    assert_int32(1, choiceView->fHideChoicesCalled, "HideChoices should be called");
    assert_int32(-1, style.SelectedChoiceIndex(), "Selection should be reset");
    assert_bool(false, choiceView->fChoicesAreShown, "Choices should not be shown anymore");
}

void testApplyChoiceEdgeCases() {
    printf("Testing BDefaultCompletionStyle::ApplyChoice edge cases...\n");

    // Case 1: No selection
    {
        MockEditView* editView = new MockEditView();
        MockChoiceModel* choiceModel = new MockChoiceModel();
        MockChoiceView* choiceView = new MockChoiceView();
        MockPatternSelector* patternSelector = new MockPatternSelector();
        BDefaultCompletionStyle style(editView, choiceModel, choiceView, patternSelector);

        choiceModel->AddChoice("choice");
        style.EditViewStateChanged(true);
        style.Select(-1);

        editView->fSetEditViewStateCalled = 0;
        style.ApplyChoice();
        assert_int32(0, editView->fSetEditViewStateCalled, "ApplyChoice should do nothing if no choice is selected");
    }

    // Case 2: MY_NULLPTR EditView
    {
        MockChoiceModel* choiceModel = new MockChoiceModel();
        MockChoiceView* choiceView = new MockChoiceView();
        MockPatternSelector* patternSelector = new MockPatternSelector();
        BDefaultCompletionStyle style(MY_NULLPTR, choiceModel, choiceView, patternSelector);

        choiceModel->AddChoice("choice");
        style.Select(0);
        style.ApplyChoice();
        // Should not crash
        assert_int32(0, choiceView->fHideChoicesCalled, "HideChoices should not be called if EditView is MY_NULLPTR");
    }
}

void testCancelChoiceEdgeCases() {
    printf("Testing BDefaultCompletionStyle::CancelChoice edge cases...\n");

    // Case 1: Choices are not shown
    {
        MockEditView* editView = new MockEditView();
        MockChoiceModel* choiceModel = new MockChoiceModel();
        MockChoiceView* choiceView = new MockChoiceView();
        MockPatternSelector* patternSelector = new MockPatternSelector();
        BDefaultCompletionStyle style(editView, choiceModel, choiceView, patternSelector);

        choiceView->fChoicesAreShown = false;
        editView->fSetEditViewStateCalled = 0;
        style.CancelChoice();
        assert_int32(0, editView->fSetEditViewStateCalled, "CancelChoice should do nothing if choices are not shown");
    }
}

void testSelect() {
    printf("Testing BDefaultCompletionStyle::Select...\n");

    MockEditView* editView = new MockEditView();
    MockChoiceModel* choiceModel = new MockChoiceModel();
    MockChoiceView* choiceView = new MockChoiceView();
    MockPatternSelector* patternSelector = new MockPatternSelector();

    BDefaultCompletionStyle style(editView, choiceModel, choiceView, patternSelector);

    choiceModel->AddChoice("c1");
    choiceModel->AddChoice("c2");

    // Success case: select 0
    assert_bool(true, style.Select(0), "Select(0) should return true");
    assert_int32(0, style.SelectedChoiceIndex(), "Selected index should be 0");
    assert_int32(0, choiceView->fSelectedIndex, "ChoiceView should be at 0");

    // Already selected case
    assert_bool(false, style.Select(0), "Select(0) again should return false");

    // Success case: select 1
    assert_bool(true, style.Select(1), "Select(1) should return true");
    assert_int32(1, style.SelectedChoiceIndex(), "Selected index should be 1");

    // Success case: select -1 (deselect)
    assert_bool(true, style.Select(-1), "Select(-1) should return true");
    assert_int32(-1, style.SelectedChoiceIndex(), "Selected index should be -1");

    // Out of bounds: too small
    assert_bool(false, style.Select(-2), "Select(-2) should return false");

    // Out of bounds: too large
    assert_bool(false, style.Select(2), "Select(2) should return false (count is 2)");

    // MY_NULLPTR ChoiceModel
    {
        // CompletionStyle takes ownership and deletes these
        BDefaultCompletionStyle styleNullModel(new MockEditView(), 0, new MockChoiceView(), new MockPatternSelector());
        assert_bool(false, styleNullModel.Select(0), "Select(0) should return false if ChoiceModel is MY_NULLPTR");
    }

    // MY_NULLPTR ChoiceView
    {
        MockChoiceModel* model = new MockChoiceModel();
        model->AddChoice("c1");
        // CompletionStyle takes ownership and deletes these
        BDefaultCompletionStyle styleNullView(new MockEditView(), model, 0, new MockPatternSelector());
        assert_bool(false, styleNullView.Select(0), "Select(0) should return false if ChoiceView is MY_NULLPTR");
    }
}

void testSelectNextExhaustive() {
    printf("Testing BDefaultCompletionStyle::SelectNext exhaustive...\n");

    // CompletionStyle takes ownership and deletes these in its destructor
    MockEditView* editView = new MockEditView();
    MockChoiceModel* choiceModel = new MockChoiceModel();
    MockChoiceView* choiceView = new MockChoiceView();
    MockPatternSelector* patternSelector = new MockPatternSelector();

    BDefaultCompletionStyle style(editView, choiceModel, choiceView, patternSelector);

    choiceModel->AddChoice("c1");
    choiceModel->AddChoice("c2");

    // Start at -1
    assert_int32(-1, style.SelectedChoiceIndex(), "Initially nothing selected");
    assert_bool(false, style.IsChoiceSelected(), "IsChoiceSelected should be false");

    // SelectNext(false): -1 -> 0
    choiceView->fSelectChoiceAtCalled = 0;
    assert_bool(true, style.SelectNext(false), "SelectNext(false) from -1 should return true");
    assert_int32(0, style.SelectedChoiceIndex(), "Should select index 0");
    assert_bool(true, style.IsChoiceSelected(), "IsChoiceSelected should be true");
    assert_int32(0, choiceView->fSelectedIndex, "ChoiceView should be at 0");
    assert_int32(1, choiceView->fSelectChoiceAtCalled, "ChoiceView::SelectChoiceAt called once");

    // SelectNext(false): 0 -> 1
    choiceView->fSelectChoiceAtCalled = 0;
    assert_bool(true, style.SelectNext(false), "SelectNext(false) from 0 should return true");
    assert_int32(1, style.SelectedChoiceIndex(), "Should select index 1");
    assert_int32(1, choiceView->fSelectedIndex, "ChoiceView should be at 1");
    assert_int32(1, choiceView->fSelectChoiceAtCalled, "ChoiceView::SelectChoiceAt called once");

    // SelectNext(false): 1 -> 1 (at last)
    choiceView->fSelectChoiceAtCalled = 0;
    assert_bool(false, style.SelectNext(false), "SelectNext(false) from 1 should return false");
    assert_int32(1, style.SelectedChoiceIndex(), "Should stay at index 1");
    assert_int32(0, choiceView->fSelectChoiceAtCalled, "ChoiceView::SelectChoiceAt should NOT be called");

    // SelectNext(true): 1 -> 0 (wrap)
    choiceView->fSelectChoiceAtCalled = 0;
    assert_bool(true, style.SelectNext(true), "SelectNext(true) from 1 should return true (wrap)");
    assert_int32(0, style.SelectedChoiceIndex(), "Should wrap to 0");
    assert_int32(0, choiceView->fSelectedIndex, "ChoiceView should be at 0");
    assert_int32(1, choiceView->fSelectChoiceAtCalled, "ChoiceView::SelectChoiceAt called once");

    // Boundary and invalid index tests
    style.Select(-1); // reset
    assert_bool(false, style.Select(-2), "Select(-2) should return false");
    assert_int32(-1, style.SelectedChoiceIndex(), "Index should still be -1");

    // Let's test a larger list
    choiceModel->AddChoice("c3"); // Now 3 items: 0, 1, 2
    style.Select(2);
    assert_bool(true, style.SelectNext(true), "SelectNext(true) from 2 should wrap to 0");
    assert_int32(0, style.SelectedChoiceIndex(), "Index should be 0");
}

void testEditViewStateChanged() {
    printf("Testing BDefaultCompletionStyle::EditViewStateChanged...\n");

    // CompletionStyle takes ownership and deletes these in its destructor
    std::unique_ptr<MockEditView> editView(new MockEditView());
    std::unique_ptr<MockChoiceModel> choiceModel(new MockChoiceModel());
    std::unique_ptr<MockChoiceView> choiceView(new MockChoiceView());
    std::unique_ptr<MockPatternSelector> patternSelector(new MockPatternSelector());

    // Pointers for internal test access before ownership transfer
    MockChoiceModel* choiceModelPtr = choiceModel.get();
    MockChoiceView* choiceViewPtr = choiceView.get();
    MockEditView* editViewPtr = editView.get();
    MockPatternSelector* patternSelectorPtr = patternSelector.get();

    BDefaultCompletionStyle style(editView.release(), choiceModel.release(),
        choiceView.release(), patternSelector.release());

    // Test case 1: No change in text
    editViewPtr->fText = "initial";
    style.EditViewStateChanged(true); // first call sets fFullEnteredText

    choiceViewPtr->fShowChoicesCalled = 0;
    choiceViewPtr->fHideChoicesCalled = 0;
    choiceModelPtr->fFetchChoicesForCalled = 0;

    style.EditViewStateChanged(true);
    assert_int32(0, choiceModelPtr->fFetchChoicesForCalled, "Should return early if text has not changed");

    // Test case 2: updateChoices = false
    editViewPtr->fText = "changed";
    style.EditViewStateChanged(false);
    assert_int32(0, choiceModelPtr->fFetchChoicesForCalled, "Should not fetch choices if updateChoices is false");
    // But fFullEnteredText should be updated, so a subsequent call with same text returns early
    style.EditViewStateChanged(true);
    assert_int32(0, choiceModelPtr->fFetchChoicesForCalled, "Should return early even if updateChoices=true if text was updated in previous call");

    // Test case 3: updateChoices = true, multiple choices
    editViewPtr->fText = "multiple";
    editViewPtr->fCaretPos = 8;
    patternSelectorPtr->fStart = 0;
    patternSelectorPtr->fLength = 8;
    choiceModelPtr->ClearChoices();
    choiceModelPtr->AddChoice("multiple1");
    choiceModelPtr->AddChoice("multiple2");

    choiceViewPtr->fShowChoicesCalled = 0;
    choiceViewPtr->fSelectChoiceAtCalled = 0;
    style.Select(1); // Set some selection to see if it gets reset

    style.EditViewStateChanged(true);

    assert_int32(1, choiceModelPtr->fFetchChoicesForCalled, "Should fetch choices");
    assert_string_equal("multiple", choiceModelPtr->fLastPattern.String(), "Should fetch choices for correct pattern");
    assert_int32(-1, style.SelectedChoiceIndex(), "Selection should be reset to -1");
    assert_int32(1, choiceViewPtr->fShowChoicesCalled, "ShowChoices should be called when > 1 choices");
    assert_int32(-1, choiceViewPtr->fSelectedIndex, "ChoiceView should select -1");

    // Test case 4: single choice, matches pattern exactly (case-insensitive)
    editViewPtr->fText = "apple";
    editViewPtr->fCaretPos = 5;
    patternSelectorPtr->fStart = 0;
    patternSelectorPtr->fLength = 5;
    choiceModelPtr->ClearChoices();
    choiceModelPtr->AddChoice("Apple"); // Exact match except case

    choiceViewPtr->fHideChoicesCalled = 0;
    choiceViewPtr->fShowChoicesCalled = 0;

    style.EditViewStateChanged(true);

    assert_int32(1, choiceViewPtr->fHideChoicesCalled, "HideChoices should be called when single choice matches pattern exactly");
    assert_int32(0, choiceViewPtr->fShowChoicesCalled, "ShowChoices should NOT be called when single choice matches pattern exactly");

    // Test case 5: single choice, does NOT match pattern exactly
    editViewPtr->fText = "app";
    editViewPtr->fCaretPos = 3;
    patternSelectorPtr->fStart = 0;
    patternSelectorPtr->fLength = 3;
    choiceModelPtr->ClearChoices();
    choiceModelPtr->AddChoice("apple");

    choiceViewPtr->fShowChoicesCalled = 0;

    style.EditViewStateChanged(true);

    assert_int32(1, choiceViewPtr->fShowChoicesCalled, "ShowChoices should be called when single choice DOES NOT match pattern exactly");

    // Test case 6: no choices
    editViewPtr->fText = "nothing";
    editViewPtr->fCaretPos = 7;
    patternSelectorPtr->fStart = 0;
    patternSelectorPtr->fLength = 7;
    choiceModelPtr->ClearChoices();

    choiceViewPtr->fHideChoicesCalled = 0;

    style.EditViewStateChanged(true);

    assert_int32(1, choiceViewPtr->fHideChoicesCalled, "HideChoices should be called when no choices are found");
}

int main() {
    testSelect();
    testSelectPrevious();
    testSelectNext();
    testSelectNextExhaustive();
    testSingleChoice();
    testEdgeCases();
    testApplyChoice();
    testCancelChoice();
    testApplyChoiceEdgeCases();
    testCancelChoiceEdgeCases();
    testEditViewStateChanged();

    if (gTestFailures > 0) {
        printf("\nFinished running tests: %d failures\n", gTestFailures);
        return 1;
    }

    printf("\nAll tests passed!\n");
    return 0;
}
