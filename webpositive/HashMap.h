#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <map>
#include <SupportDefs.h>

template<typename Key, typename Value>
class HashMap {
public:
    struct Entry {
        Key key;
        Value value;
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
        const Entry& Next() {
            fCurrentEntry.key = fIt->first;
            fCurrentEntry.value = fIt->second;
            ++fIt;
            return fCurrentEntry;
        }

    private:
        MapIterator fIt;
        MapIterator fEnd;
        Entry fCurrentEntry;
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
