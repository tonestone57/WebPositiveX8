#ifndef _COLUMN_LIST_VIEW_H
#define _COLUMN_LIST_VIEW_H

#include "ListView.h"
#include <vector>

class BField {
public:
    virtual ~BField() {}
};

class BStringField : public BField {
public:
    BStringField(const char* string) : fString(string ? string : "") {}
    const char* String() const { return fString.String(); }
private:
    BString fString;
};

class BDateField : public BField {
public:
    BDateField(time_t* t) : fTime(t ? *t : 0) {}
    time_t UnixTime() const { return fTime; }
private:
    time_t fTime;
};

class BRow {
public:
    BRow() {}
    virtual ~BRow() {
        for (auto field : fFields) delete field;
    }
    void SetField(BField* field, int32 logicalColumn) {
        if (logicalColumn >= (int32)fFields.size())
            fFields.resize(logicalColumn + 1, nullptr);
        delete fFields[logicalColumn];
        fFields[logicalColumn] = field;
    }
    BField* GetField(int32 logicalColumn) {
        if (logicalColumn >= 0 && logicalColumn < (int32)fFields.size())
            return fFields[logicalColumn];
        return nullptr;
    }
private:
    std::vector<BField*> fFields;
};

class BColumn {
public:
    BColumn(const char* title, float width, float minWidth, float maxWidth, uint32 truncate) {}
    virtual ~BColumn() {}
    void DrawString(const char* string, BView* parent, BRect rect) {}
};

class BStringColumn : public BColumn {
public:
    BStringColumn(const char* title, float width, float minWidth, float maxWidth, uint32 truncate)
        : BColumn(title, width, minWidth, maxWidth, truncate) {}
};

class BDateColumn : public BColumn {
public:
    BDateColumn(const char* title, float width, float minWidth, float maxWidth)
        : BColumn(title, width, minWidth, maxWidth, 0) {}
    virtual void DrawField(BField* field, BRect rect, BView* parent) {}
};

class BColumnListView : public BView {
public:
    BColumnListView(const char* name, uint32 flags, border_style border, bool showHorizontalScrollbar) {}
    virtual ~BColumnListView() {
        for (auto row : fRows) delete row;
    }
    void AddColumn(BColumn* column, int32 logicalIndex) { fColumns.push_back(column); }
    void AddRow(BRow* row, BRow* parent = nullptr) { fRows.push_back(row); }
    void AddRow(BRow* row, int32 index, BRow* parent = nullptr) { fRows.insert(fRows.begin() + index, row); }
    void RemoveRow(BRow* row) {
        for (auto it = fRows.begin(); it != fRows.end(); ++it) {
            if (*it == row) { fRows.erase(it); break; }
        }
    }
    void Clear() {
        for (auto row : fRows) delete row;
        fRows.clear();
    }
    BRow* RowAt(int32 index) const {
        if (index >= 0 && index < (int32)fRows.size()) return fRows[index];
        return nullptr;
    }
    BRow* CurrentSelection(BRow* after = nullptr) const {
        if (fRows.empty()) return nullptr;
        if (after == nullptr) return fRows[0]; // Simple mock: always select first
        for (size_t i = 0; i < fRows.size() - 1; ++i) {
            if (fRows[i] == after) return fRows[i+1];
        }
        return nullptr;
    }
    float StringWidth(const char* string) const { return 100.0f; }

private:
    std::vector<BColumn*> fColumns;
    std::vector<BRow*> fRows;
};

#endif
