#ifndef _MOCK_URL_CONTEXT_H
#define _MOCK_URL_CONTEXT_H
template<typename T> class BReference {
public:
    BReference() : fObject(MY_NULLPTR) {}
    BReference(T* object) : fObject(object) { if (fObject) fObject->AcquireReference(); }
    BReference(const BReference<T>& other) : fObject(other.fObject) { if (fObject) fObject->AcquireReference(); }
    ~BReference() { if (fObject) fObject->ReleaseReference(); }
    T* Get() const { return fObject; }
    T* operator->() const { return fObject; }
private:
    T* fObject;
};
namespace BPrivate { namespace Network { class BUrlContext {}; } }
#endif
