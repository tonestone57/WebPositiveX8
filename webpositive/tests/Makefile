# Makefile for running tests on non-Haiku systems using mocks

CXX = g++
CXXFLAGS = -I. -Ihaiku_mocks -I../autocompletion -I.. -Wall -Wno-multichar

TESTS = CompletionStyleTest BrowsingHistoryTest DownloadSpeedCalculatorTest BrowsingHistoryItemTest SettingsFileTest URLChoiceTest PatternSelectorTest BrowsingHistoryChoiceModelTest CredentialsTest ConsoleWindowTest

all: $(TESTS)

CompletionStyleTest: CompletionStyleTest.cpp ../autocompletion/AutoCompleterDefaultImpl.cpp ../autocompletion/AutoCompleter.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

BrowsingHistoryTest: BrowsingHistoryTest.cpp ../BrowsingHistory.cpp ../SettingsKeys.cpp ../SettingsFile.cpp haiku_mocks/Mocks.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

DownloadSpeedCalculatorTest: DownloadSpeedCalculatorTest.cpp ../DownloadSpeedCalculator.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

BrowsingHistoryItemTest: BrowsingHistoryItemTest.cpp ../BrowsingHistory.cpp ../SettingsKeys.cpp ../SettingsFile.cpp haiku_mocks/Mocks.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

SettingsFileTest: SettingsFileTest.cpp haiku_mocks/Mocks.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

URLChoiceTest: URLChoiceTest.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

PatternSelectorTest: PatternSelectorTest.cpp ../autocompletion/AutoCompleterDefaultImpl.cpp ../autocompletion/AutoCompleter.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

BrowsingHistoryChoiceModelTest: BrowsingHistoryChoiceModelTest.cpp ../autocompletion/BrowsingHistoryChoiceModel.cpp ../BrowsingHistory.cpp ../support/BaseURL.cpp ../SettingsFile.cpp ../autocompletion/AutoCompleter.cpp ../autocompletion/AutoCompleterDefaultImpl.cpp ../SettingsKeys.cpp haiku_mocks/Mocks.cpp
	$(CXX) $(CXXFLAGS) -I../support $^ -o $@

CredentialsTest: CredentialsTest.cpp ../CredentialsStorage.cpp ../SettingsFile.cpp ../SettingsKeys.cpp haiku_mocks/Mocks.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

ConsoleWindowTest: ConsoleWindowTest.cpp haiku_mocks/Mocks.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

run: $(TESTS)
	./CompletionStyleTest
	./BrowsingHistoryTest
	./DownloadSpeedCalculatorTest
	./BrowsingHistoryItemTest
	./SettingsFileTest
	./URLChoiceTest
	./PatternSelectorTest
	./BrowsingHistoryChoiceModelTest
	./CredentialsTest
	./ConsoleWindowTest

clean:
	rm -f $(TESTS)

.PHONY: all run clean
