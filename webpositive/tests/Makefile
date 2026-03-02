# Makefile for running tests on non-Haiku systems using mocks

CXX = g++
CXXFLAGS = -I. -Ihaiku_mocks -I../autocompletion -I../support -I.. -Wall -Wno-multichar

TESTS = CompletionStyleTest BrowsingHistoryTest DownloadSpeedCalculatorTest BrowsingHistoryItemTest SettingsFileTest URLChoiceTest PatternSelectorTest BrowsingHistoryChoiceModelTest CredentialsTest ConsoleWindowTest CookieWindowTest BookmarkBarTest TabManagerTest

all: $(TESTS)

CompletionStyleTest: CompletionStyleTest.cpp ../autocompletion/AutoCompleterDefaultImpl.cpp ../autocompletion/AutoCompleter.cpp haiku_mocks/Mocks.cpp
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

PatternSelectorTest: PatternSelectorTest.cpp ../autocompletion/AutoCompleterDefaultImpl.cpp ../autocompletion/AutoCompleter.cpp haiku_mocks/Mocks.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

BrowsingHistoryChoiceModelTest: BrowsingHistoryChoiceModelTest.cpp ../autocompletion/BrowsingHistoryChoiceModel.cpp ../BrowsingHistory.cpp ../support/BaseURL.cpp ../SettingsFile.cpp ../autocompletion/AutoCompleter.cpp ../autocompletion/AutoCompleterDefaultImpl.cpp ../SettingsKeys.cpp haiku_mocks/Mocks.cpp
	$(CXX) $(CXXFLAGS) -I../support $^ -o $@

CredentialsTest: CredentialsTest.cpp ../CredentialsStorage.cpp ../SettingsFile.cpp ../SettingsKeys.cpp haiku_mocks/Mocks.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

ConsoleWindowTest: ConsoleWindowTest.cpp haiku_mocks/Mocks.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

CookieWindowTest: CookieWindowTest.cpp haiku_mocks/Mocks.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

BookmarkBarTest: BookmarkBarTest.cpp haiku_mocks/Mocks.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

TabManagerTest: TabManagerTest.cpp ../tabview/TabManager.cpp ../tabview/TabView.cpp ../tabview/TabContainerView.cpp haiku_mocks/Mocks.cpp
	$(CXX) $(CXXFLAGS) -I../tabview $^ -o $@

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
	./CookieWindowTest
	./BookmarkBarTest
	./TabManagerTest

clean:
	rm -f $(TESTS)

.PHONY: all run clean
