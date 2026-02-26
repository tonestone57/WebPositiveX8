#ifndef _MOCK_HASH_MAP_H
#define _MOCK_HASH_MAP_H
#include <map>
template<typename Key, typename Value>
class HashMap {
public:
    class Entry {
    public:
        Key key;
        Value value;
    };
    class Iterator {
    public:
        bool HasNext() { return false; }
        const Entry& Next() { static Entry e; return e; }
    };
    bool ContainsKey(const Key& key) const { return false; }
    status_t Put(const Key& key, const Value& value) { return B_OK; }
    Value Get(const Key& key) const { return Value(); }
    void Remove(const Key& key) {}
    Iterator GetIterator() { return Iterator(); }
};
#endif
