#ifndef _URL_H
#define _URL_H
#include "String.h"
namespace BPrivate { namespace Network {
class BUrl {
public:
    BUrl() {}
    BUrl(const char* url) : fUrl(url) {}
    const char* String() const { return fUrl.String(); }
private:
    BString fUrl;
};
} }
using BPrivate::Network::BUrl;
#endif
