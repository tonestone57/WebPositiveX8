#ifndef _MOCK_URL_CONTEXT_H
#define _MOCK_URL_CONTEXT_H
template<typename T> class BReference {
public:
    T* Get() const { return nullptr; }
};
namespace BPrivate { namespace Network { class BUrlContext {}; } }
#endif
