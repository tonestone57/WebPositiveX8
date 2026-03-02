#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <map>
#include <SupportDefs.h>

template<typename Key, typename Value>
class HashMap {
public:
    struct Entry {
        const Key& first;
        Value& second;
        Entry(const Key& k, Value& v) : first(k), second(v) {}
    };

    typedef typename std::map<Key, Value>::iterator MapIterator;
    typedef typename std::map<Key, Value>::const_iterator ConstMapIterator;

    class Iterator {
    public:
        Iterator(MapIterator it, MapIterator end) : fIt(it), fEnd(end) {}

        bool operator==(const Iterator& other) const { return fIt == other.fIt; }
        bool operator!=(const Iterator& other) const { return fIt != other.fIt; }

        Iterator& operator++() { ++fIt; return *this; }

        MapIterator operator->() const { return fIt; }

        bool HasNext() const { return fIt != fEnd; }

        const Entry Next() {
            Entry entry(fIt->first, fIt->second);
            ++fIt;
            return entry;
        }

        const Key& key() const { return fIt->first; }
        Value& value() const { return fIt->second; }

    private:
        MapIterator fIt;
        MapIterator fEnd;
    };

    bool ContainsKey(const Key& key) const {
        return fMap.find(key) != fMap.end();
    }

    status_t Put(const Key& key, const Value& value) {
        fMap[key] = value;
        return B_OK;
    }

    Value Get(const Key& key) const {
        ConstMapIterator it = fMap.find(key);
        if (it != fMap.end())
            return it->second;
        return Value();
    }

    void Remove(const Key& key) {
        fMap.erase(key);
    }

    void RemoveAll() {
        fMap.clear();
    }

    Iterator GetIterator() {
        return Iterator(fMap.begin(), fMap.end());
    }

    Iterator find(const Key& key) {
        return Iterator(fMap.find(key), fMap.end());
    }

    Iterator end() {
        return Iterator(fMap.end(), fMap.end());
    }

    Value& operator[](const Key& key) {
        return fMap[key];
    }

private:
    std::map<Key, Value> fMap;
};

#endif
