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

    class Iterator {
    public:
        Iterator(typename std::map<Key, Value>::iterator it, typename std::map<Key, Value>::iterator end)
            : fIt(it), fEnd(end) {}

        bool HasNext() const { return fIt != fEnd; }

        const Entry& Next() {
            fCurrentEntry.key = fIt->first;
            fCurrentEntry.value = fIt->second;
            ++fIt;
            return fCurrentEntry;
        }

    private:
        typename std::map<Key, Value>::iterator fIt;
        typename std::map<Key, Value>::iterator fEnd;
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
        typename std::map<Key, Value>::const_iterator it = fMap.find(key);
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

    // Add find for compatibility with existing code if needed
    Iterator find(const Key& key) {
        return Iterator(fMap.find(key), fMap.end());
    }

    Iterator end() {
        return Iterator(fMap.end(), fMap.end());
    }

private:
    std::map<Key, Value> fMap;
};

#endif
