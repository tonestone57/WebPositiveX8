#ifndef _MOCK_NETWORK_COOKIE_JAR_H
#define _MOCK_NETWORK_COOKIE_JAR_H

#include "String.h"
#include "Message.h"
#include <vector>

namespace BPrivate {
namespace Network {

class BNetworkCookie {
public:
    BNetworkCookie() : fExpirationDate(0), fSecure(false), fHttpOnly(false), fHostOnly(false) {}
    BNetworkCookie(const char* name, const char* value) : fName(name), fValue(value), fExpirationDate(0), fSecure(false), fHttpOnly(false), fHostOnly(false) {}

    const BString& Name() const { return fName; }
    const BString& Value() const { return fValue; }
    const BString& Domain() const { return fDomain; }
    const BString& Path() const { return fPath; }
    time_t ExpirationDate() const { return fExpirationDate; }
    bool Secure() const { return fSecure; }
    bool HttpOnly() const { return fHttpOnly; }
    bool IsHostOnly() const { return fHostOnly; }

    void SetName(const char* name) { fName = name; }
    void SetValue(const char* value) { fValue = value; }
    void SetDomain(const char* domain) { fDomain = domain; }
    void SetPath(const char* path) { fPath = path; }
    void SetExpirationDate(time_t date) { fExpirationDate = date; }
    void SetSecure(bool secure) { fSecure = secure; }
    void SetHttpOnly(bool httpOnly) { fHttpOnly = httpOnly; }
    void SetHostOnly(bool hostOnly) { fHostOnly = hostOnly; }

private:
    BString fName;
    BString fValue;
    BString fDomain;
    BString fPath;
    time_t fExpirationDate;
    bool fSecure;
    bool fHttpOnly;
    bool fHostOnly;
};

class BNetworkCookieJar {
public:
    BNetworkCookieJar() {}
    BNetworkCookieJar(BMessage* archive) { Unarchive(archive); }

    void AddCookie(const BNetworkCookie& cookie) {
        for (auto& c : fCookies) {
            if (c.Name() == cookie.Name() && c.Domain() == cookie.Domain() && c.Path() == cookie.Path()) {
                c = cookie;
                return;
            }
        }
        fCookies.push_back(cookie);
    }

    void Unarchive(BMessage* archive) {
        if (!archive) return;
        BMessage cookiesMsg;
        if (archive->FindMessage("cookies", &cookiesMsg) == B_OK) {
            BMessage cookieMsg;
            for (int32 i = 0; cookiesMsg.FindMessage("cookie", i, &cookieMsg) == B_OK; i++) {
                BNetworkCookie cookie;
                BString s;
                if (cookieMsg.FindString("name", &s) == B_OK) cookie.SetName(s.String());
                if (cookieMsg.FindString("value", &s) == B_OK) cookie.SetValue(s.String());
                if (cookieMsg.FindString("domain", &s) == B_OK) cookie.SetDomain(s.String());
                if (cookieMsg.FindString("path", &s) == B_OK) cookie.SetPath(s.String());
                AddCookie(cookie);
            }
        }
    }

    status_t Archive(BMessage* archive) const { return B_OK; }
    void PurgeForExit() {}

    class Iterator {
    public:
        Iterator(const std::vector<BNetworkCookie>& cookies) : fCookies(cookies), fIndex(0) {}
        const BNetworkCookie* Next() {
            if (fIndex < fCookies.size()) return &fCookies[fIndex++];
            return MY_NULLPTR;
        }
    private:
        const std::vector<BNetworkCookie>& fCookies;
        size_t fIndex;
    };

    Iterator GetIterator() const { return Iterator(fCookies); }

private:
    std::vector<BNetworkCookie> fCookies;
};

} // namespace Network
} // namespace BPrivate

#endif
