#ifndef _MOCK_KEY_STORE_H
#define _MOCK_KEY_STORE_H
#include "SupportDefs.h"
enum {
    B_KEY_PURPOSE_WEB
};
class BPasswordKey {
public:
    BPasswordKey() {}
    BPasswordKey(const char* password, uint32 purpose, const char* identifier, const char* secondaryIdentifier) {}
    const char* Password() const { return ""; }
};
class BKeyStore {
public:
    status_t AddKey(const BPasswordKey& key) { return B_OK; }
    status_t RemoveKey(uint32 purpose, const char* identifier, const char* secondaryIdentifier) { return B_OK; }
    status_t GetKey(uint32 purpose, const char* identifier, const char* secondaryIdentifier, bool secondaryIdentifierOptional, BPasswordKey& key) { return B_ERROR; }
};
#endif
