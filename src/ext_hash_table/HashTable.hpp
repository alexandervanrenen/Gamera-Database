#pragma once

#include <vector>
#include <cmath>
#include <utility>
#include <bitset>
#include "ext_hash_table/Bucket.hpp"
#include <memory>

namespace dbi {

template<typename Key, typename Value, typename Hash = std::hash<Key>>
class HashTable {
public:
    HashTable() {
        numRelevantBits = 1;
        Bucket<Key,Value>* initialBucket = new Bucket<Key,Value>();
        directory.resize(2);
        directory[0] = std::shared_ptr<Bucket<Key,Value>>(initialBucket);
        directory[1] = std::shared_ptr<Bucket<Key,Value>>(initialBucket);
    }
    
    ~HashTable() {
        
    }

    void insert(const Key& key, const Value& value){
        uint16_t dirIndex = computeDirectoryIndex(key);
        bool success = directory[dirIndex]->insert(key,value);
        if(!success) {
            // TODO: if !success enlarge directory
            throw;
        }
    }
    
    Value get(const Key& key) {
        uint16_t dirIndex = computeDirectoryIndex(key);
        return directory[dirIndex]->get(key);
    }
    
    void remove(const Key& key) {
        throw;
    }
private:
    
    std::vector<std::shared_ptr<Bucket<Key,Value>>> directory;
    uint16_t numRelevantBits;
    
    std::vector<std::shared_ptr<Bucket<Key,Value>>> createDirectory(uint16_t relevantBits){
        return std::vector<std::shared_ptr<Bucket<Key,Value>>>(pow(2, relevantBits));
    }
    
    uint16_t computeDirectoryIndex(Key key) {
        uint64_t hash = std::hash<Key>()(key);
        // Shift left -> append N 0, then subtract one -> result is N bits of 1 -> result as int is index to insert pair
        return hash & ((1 << numRelevantBits) -1);
    }
    
};

}

