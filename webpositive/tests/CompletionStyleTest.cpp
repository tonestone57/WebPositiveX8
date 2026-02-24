#include <stdio.h>
#include <vector>
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
    virtual BRect GetAdjustmentFrame() { return BRect(); }
    virtual void GetEditViewState(BString& text, int32* caretPos) {}
    virtual void SetEditViewState(const BString& text, int32 caretPos, int32 selectionLength = 0) {}
};

class MockChoiceModel : public BAutoCompleter::ChoiceModel {
public:
    MockChoiceModel() {}
    virtual ~MockChoiceModel() {
        for (auto choice : fChoices)
            delete choice;
    }
    virtual void FetchChoicesFor(const BString& pattern) {}
    virtual int32 CountChoices() const { return fChoices.size(); }
    virtual const BAutoCompleter::Choice* ChoiceAt(int32 index) const {
        if (index >= 0 && index < (int32)fChoices.size())
            return fChoices[index];
        return NULL;
    }

    void AddChoice(const char* text) {
        fChoices.push_back(new BAutoCompleter::Choice(text, text, 0, 0));
    }

private:
    std::vector<BAutoCompleter::Choice*> fChoices;
};

class MockChoiceView : public BAutoCompleter::ChoiceView {
public:
    MockChoiceView() : fSelectedIndex(-1) {}
    virtual void SelectChoiceAt(int32 index) {
        fSelectedIndex = index;
    }
    virtual void ShowChoices(BAutoCompleter::CompletionStyle* completer) {}
    virtual void HideChoices() {}
    virtual bool ChoicesAreShown() { return false; }
    virtual int32 CountVisibleChoices() const { return 0; }

    int32 fSelectedIndex;
};

class MockPatternSelector : public BAutoCompleter::PatternSelector {
public:
    virtual void SelectPatternBounds(const BString& text, int32 caretPos, int32* start, int32* length) {}
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
    assert_bool(true, style.SelectPrevious(false), "SelectPrevious(false) from -1 should return true");
    assert_int32(0, style.SelectedChoiceIndex(), "SelectPrevious(false) from -1 should select 0");

    // Test case 3: Initial state (-1), wrap=true
    style.Select(-1);
    assert_bool(true, style.SelectPrevious(true), "SelectPrevious(true) from -1 should return true");
    assert_int32(2, style.SelectedChoiceIndex(), "SelectPrevious(true) from -1 should select 2 (last item)");

    // Test case 4: From middle (index 1)
    style.Select(1);
    assert_bool(true, style.SelectPrevious(false), "SelectPrevious from 1 should return true");
    assert_int32(0, style.SelectedChoiceIndex(), "SelectPrevious from 1 should select 0");

    // Test case 5: From 0, wrap=false
    style.Select(0);
    assert_bool(false, style.SelectPrevious(false), "SelectPrevious(false) from 0 should return false (already at 0)");
    assert_int32(0, style.SelectedChoiceIndex(), "SelectPrevious(false) from 0 should stay at 0");

    // Test case 6: From 0, wrap=true
    style.Select(0);
    assert_bool(true, style.SelectPrevious(true), "SelectPrevious(true) from 0 should return true (wraps)");
    assert_int32(2, style.SelectedChoiceIndex(), "SelectPrevious(true) from 0 should wrap to 2");
}

void testSelectNext() {
    printf("Testing BDefaultCompletionStyle::SelectNext...\n");

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
    assert_bool(true, style.SelectNext(false), "SelectNext(false) from -1 should return true");
    assert_int32(0, style.SelectedChoiceIndex(), "SelectNext(false) from -1 should select 0");

    // Test case 3: Initial state (-1), wrap=true
    style.Select(-1);
    assert_bool(true, style.SelectNext(true), "SelectNext(true) from -1 should return true");
    assert_int32(0, style.SelectedChoiceIndex(), "SelectNext(true) from -1 should select 0");

    // Test case 4: From middle (index 1)
    style.Select(1);
    assert_bool(true, style.SelectNext(false), "SelectNext from 1 should return true");
    assert_int32(2, style.SelectedChoiceIndex(), "SelectNext from 1 should select 2");

    // Test case 5: From last (index 2), wrap=false
    style.Select(2);
    assert_bool(false, style.SelectNext(false), "SelectNext(false) from 2 should return false (already at last)");
    assert_int32(2, style.SelectedChoiceIndex(), "SelectNext(false) from 2 should stay at 2");

    // Test case 6: From last (index 2), wrap=true
    style.Select(2);
    assert_bool(true, style.SelectNext(true), "SelectNext(true) from 2 should return true (wraps)");
    assert_int32(0, style.SelectedChoiceIndex(), "SelectNext(true) from 2 should wrap to 0");
}

void testSingleChoice() {
    printf("Testing BDefaultCompletionStyle with single choice...\n");

    MockEditView* editView = new MockEditView();
    MockChoiceModel* choiceModel = new MockChoiceModel();
    MockChoiceView* choiceView = new MockChoiceView();
    MockPatternSelector* patternSelector = new MockPatternSelector();

    choiceModel->AddChoice("only");
    BDefaultCompletionStyle style(editView, choiceModel, choiceView, patternSelector);

    // SelectNext
    style.Select(-1);
    assert_bool(true, style.SelectNext(false), "SelectNext(false) from -1 with single choice should return true");
    assert_int32(0, style.SelectedChoiceIndex(), "SelectNext(false) from -1 with single choice should select 0");

    assert_bool(false, style.SelectNext(false), "SelectNext(false) from 0 with single choice should return false");
    assert_bool(false, style.SelectNext(true), "SelectNext(true) from 0 with single choice should return false");

    // SelectPrevious
    style.Select(-1);
    assert_bool(true, style.SelectPrevious(false), "SelectPrevious(false) from -1 with single choice should return true");
    assert_int32(0, style.SelectedChoiceIndex(), "SelectPrevious(false) from -1 with single choice should select 0");

    assert_bool(false, style.SelectPrevious(false), "SelectPrevious(false) from 0 with single choice should return false");
    assert_bool(false, style.SelectPrevious(true), "SelectPrevious(true) from 0 with single choice should return false");
}

int main() {
    testSelectPrevious();
    testSelectNext();
    testSingleChoice();

    if (gTestFailures > 0) {
        printf("\nFinished running tests: %d failures\n", gTestFailures);
        return 1;
    }

    printf("\nAll tests passed!\n");
    return 0;
}
