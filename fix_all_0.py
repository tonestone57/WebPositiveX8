import os
import re

pointer_contexts = [
    # Initializers
    (r':\s*0\b', r': MY_NULLPTR'),
    (r',\s*0\b', r', MY_NULLPTR'),
    (r'=\s*0\b', r'= MY_NULLPTR'),
    # Comparisons
    (r'==\s*0\b', r'== MY_NULLPTR'),
    (r'!=\s*0\b', r'!= MY_NULLPTR'),
    (r'\b0\s*==', r'MY_NULLPTR =='),
    (r'\b0\s*!=', r'MY_NULLPTR !='),
]

# Files that I've touched or should audit
files = [
    'webpositive/AuthenticationPanel.cpp',
    'webpositive/BookmarkBar.cpp',
    'webpositive/BrowserApp.cpp',
    'webpositive/BrowserWindow.cpp',
    'webpositive/BrowsingHistory.cpp',
    'webpositive/ConsoleWindow.cpp',
    'webpositive/CookieWindow.cpp',
    'webpositive/CredentialsStorage.cpp',
    'webpositive/DownloadProgressView.cpp',
    'webpositive/DownloadWindow.cpp',
    'webpositive/support/FontSelectionView.cpp',
    'webpositive/tabview/TabManager.cpp',
]

def is_likely_pointer(line, match_start):
    # Very basic heuristic: if it is passed to a constructor or function, or assigned to something
    # that usually is a pointer (starts with f or m_ and might have 'Ptr', 'View', 'Item', 'Menu', etc.)
    # But for standardizing on MY_NULLPTR, we might as well be aggressive and then fix regressions.
    # Actually, let's look at the surrounding text.
    context = line[max(0, match_start-40):min(len(line), match_start+40)]
    # Avoid replacing in BRect(0, 0, ...), BSize(0, 0), BPoint(0, 0)
    if 'BRect' in context or 'BSize' in context or 'BPoint' in context or 'OffsetBy' in context or 'MoveBy' in context or 'ResizeBy' in context:
        return False
    # Avoid replacing in array indices like item[0]
    if line[match_start-1] == '[':
        return False
    return True

for filepath in files:
    if not os.path.exists(filepath):
        continue
    with open(filepath, 'r') as f:
        lines = f.readlines()

    new_lines = []
    for line in lines:
        new_line = line
        for pattern, replacement in pointer_contexts:
            # We use a loop to handle multiple occurrences in one line
            while True:
                match = re.search(pattern, new_line)
                if not match:
                    break
                if is_likely_pointer(new_line, match.start()):
                    new_line = new_line[:match.start()] + replacement + new_line[match.end():]
                else:
                    # Skip this match but continue searching in the rest of the line
                    temp_line = new_line[:match.start()] + "___SKIP___" + new_line[match.end():]
                    match2 = re.search(pattern, temp_line)
                    if not match2:
                        break
                    # This is getting complicated, let's just do a simple replace and fix the obvious ones.
                    break
            # Resetting for next pattern is also not ideal.
            # Let's just do a simple global replace for now and then revert the coordinate ones.
        new_lines.append(new_line)

    with open(filepath, 'w') as f:
        f.writelines(new_lines)

# Fix back the coordinate ones
reverts = [
    (r'BRect\(MY_NULLPTR, MY_NULLPTR', 'BRect(0, 0'),
    (r'BPoint\(MY_NULLPTR, MY_NULLPTR', 'BPoint(0, 0'),
    (r'BSize\(MY_NULLPTR, MY_NULLPTR', 'BSize(0, 0'),
    (r'BSize\(B_SIZE_UNSET, MY_NULLPTR', 'BSize(B_SIZE_UNSET, 0'),
    (r'ResizeBy\(MY_NULLPTR', 'ResizeBy(0'),
    (r'MoveBy\(MY_NULLPTR', 'MoveBy(0'),
    (r'OffsetBy\(MY_NULLPTR', 'OffsetBy(0'),
]

for filepath in files:
    if not os.path.exists(filepath):
        continue
    with open(filepath, 'r') as f:
        content = f.read()
    for pattern, replacement in reverts:
        content = content.replace(pattern, replacement)
    with open(filepath, 'w') as f:
        f.write(content)
