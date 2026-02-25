# Makefile for running tests on non-Haiku systems using mocks

CXX = g++
CXXFLAGS = -Ihaiku_mocks -I../autocompletion -I.. -Wall -Wno-multichar

TESTS = CompletionStyleTest

all: $(TESTS)

CompletionStyleTest: CompletionStyleTest.cpp ../autocompletion/AutoCompleterDefaultImpl.cpp ../autocompletion/AutoCompleter.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

run: $(TESTS)
	./CompletionStyleTest

clean:
	rm -f $(TESTS)

.PHONY: all run clean
