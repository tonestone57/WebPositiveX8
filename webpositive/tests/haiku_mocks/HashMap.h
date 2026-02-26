#ifndef _MOCK_HASH_MAP_H
#define _MOCK_HASH_MAP_H

#include <map>
#include "SupportDefs.h"

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
        auto it = fMap.find(key);
        if (it != fMap.end())
            return it->second;
        return Value();
    }

    void Remove(const Key& key) {
        fMap.erase(key);
    }

    Iterator GetIterator() {
        return Iterator(fMap.begin(), fMap.end());
    }

private:
    std::map<Key, Value> fMap;
};

#endif
