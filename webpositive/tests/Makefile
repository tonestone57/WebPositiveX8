# Makefile for running tests on non-Haiku systems using mocks

CXX = g++
CXXFLAGS = -I. -Ihaiku_mocks -I../autocompletion -I.. -Wall -Wno-multichar

TESTS = CompletionStyleTest BrowsingHistoryTest

all: $(TESTS)

CompletionStyleTest: CompletionStyleTest.cpp ../autocompletion/AutoCompleterDefaultImpl.cpp ../autocompletion/AutoCompleter.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

BrowsingHistoryTest: BrowsingHistoryTest.cpp ../SettingsKeys.cpp SettingsFile.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

run: $(TESTS)
	./CompletionStyleTest
	./BrowsingHistoryTest

clean:
	rm -f $(TESTS)

.PHONY: all run clean
