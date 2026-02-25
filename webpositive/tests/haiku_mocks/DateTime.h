#ifndef _MOCK_DATETIME_H
#define _MOCK_DATETIME_H
#include "SupportDefs.h"
#include "Message.h"
class BDate {
public:
    BDate() : fDays(0) {}
    void AddDays(int32 days) { fDays += days; }
    bool operator<(const BDate& other) const { return fDays < other.fDays; }
    bool operator<=(const BDate& other) const { return fDays <= other.fDays; }
    bool operator>(const BDate& other) const { return fDays > other.fDays; }
    bool operator>=(const BDate& other) const { return fDays >= other.fDays; }
    bool operator==(const BDate& other) const { return fDays == other.fDays; }
    bool operator!=(const BDate& other) const { return fDays != other.fDays; }
    int32 fDays;
};
class BDateTime {
public:
    BDateTime() : fTime(0) {}
    BDateTime(BMessage* archive) : fTime(0) {}
    static BDateTime CurrentDateTime(int32 time_zone) {
        static int32 mockTime = 10000;
        BDateTime dt;
        dt.fTime = mockTime++;
        return dt;
    }
    BDate& Date() { return fDate; }
    const BDate& Date() const { return fDate; }
    status_t Archive(BMessage* archive) const { return B_OK; }
    bool operator<(const BDateTime& other) const {
        if (fDate != other.fDate) return fDate < other.fDate;
        return fTime < other.fTime;
    }
    bool operator<=(const BDateTime& other) const { return !(*this > other); }
    bool operator>(const BDateTime& other) const {
        if (fDate != other.fDate) return fDate > other.fDate;
        return fTime > other.fTime;
    }
    bool operator>=(const BDateTime& other) const { return !(*this < other); }
    bool operator==(const BDateTime& other) const { return fDate == other.fDate && fTime == other.fTime; }
    bool operator!=(const BDateTime& other) const { return !(*this == other); }
    bool IsValid() const { return true; }
    int32 fTime;
    BDate fDate;
};
#endif
