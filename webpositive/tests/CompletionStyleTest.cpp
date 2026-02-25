#include <stdio.h>
#include <vector>
#include <string.h>
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
    MockChoiceModel() {}
    virtual ~MockChoiceModel() {
        for (std::vector<BAutoCompleter::Choice*>::iterator it = fChoices.begin();
                it != fChoices.end(); ++it) {
            delete *it;
        }
    }
    virtual void FetchChoicesFor(const BString& pattern) {}
    virtual int32 CountChoices() const { return (int32)fChoices.size(); }
    virtual const BAutoCompleter::Choice* ChoiceAt(int32 index) const {
        if (index >= 0 && index < (int32)fChoices.size())
            return fChoices[index];
        return NULL;
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

private:
    std::vector<BAutoCompleter::Choice*> fChoices;
};

class MockChoiceView : public BAutoCompleter::ChoiceView {
public:
    MockChoiceView() : fSelectedIndex(-1), fSelectChoiceAtCalled(0),
        fHideChoicesCalled(0), fChoicesAreShown(false) {}
    virtual void SelectChoiceAt(int32 index) {
        fSelectedIndex = index;
        fSelectChoiceAtCalled++;
    }
    virtual void ShowChoices(BAutoCompleter::CompletionStyle* completer) {
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

    // NULL ChoiceModel
    {
        // CompletionStyle takes ownership and deletes these in its destructor
        MockEditView* editView = new MockEditView();
        MockChoiceView* choiceView = new MockChoiceView();
        MockPatternSelector* patternSelector = new MockPatternSelector();
        BDefaultCompletionStyle style(editView, NULL, choiceView, patternSelector);
        assert_bool(false, style.SelectNext(), "SelectNext should return false when ChoiceModel is NULL");
        assert_bool(false, style.SelectPrevious(), "SelectPrevious should return false when ChoiceModel is NULL");
    }

    // NULL ChoiceView
    {
        // CompletionStyle takes ownership and deletes these in its destructor
        MockEditView* editView = new MockEditView();
        MockChoiceModel* choiceModel = new MockChoiceModel();
        MockPatternSelector* patternSelector = new MockPatternSelector();
        BDefaultCompletionStyle style(editView, choiceModel, NULL, patternSelector);
        choiceModel->AddChoice("choice1");
        assert_bool(false, style.SelectNext(), "SelectNext should return false when ChoiceView is NULL");
        assert_bool(false, style.SelectPrevious(), "SelectPrevious should return false when ChoiceView is NULL");
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
    assert_bool(true, strcmp("application", editView->fLastSetText.String()) == 0, "Text should be completed to 'application'");
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

    assert_bool(true, strcmp("test application", editView->fLastSetText.String()) == 0, "Partial completion should work");
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
    assert_bool(true, strcmp("app", editView->fLastSetText.String()) == 0, "Original text should be restored");
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

    // Case 2: NULL EditView
    {
        MockChoiceModel* choiceModel = new MockChoiceModel();
        MockChoiceView* choiceView = new MockChoiceView();
        MockPatternSelector* patternSelector = new MockPatternSelector();
        BDefaultCompletionStyle style(NULL, choiceModel, choiceView, patternSelector);

        choiceModel->AddChoice("choice");
        style.Select(0);
        style.ApplyChoice();
        // Should not crash
        assert_int32(0, choiceView->fHideChoicesCalled, "HideChoices should not be called if EditView is NULL");
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

int main() {
    testSelectPrevious();
    testSelectNext();
    testSingleChoice();
    testEdgeCases();
    testApplyChoice();
    testCancelChoice();
    testApplyChoiceEdgeCases();
    testCancelChoiceEdgeCases();

    if (gTestFailures > 0) {
        printf("\nFinished running tests: %d failures\n", gTestFailures);
        return 1;
    }

    printf("\nAll tests passed!\n");
    return 0;
}
