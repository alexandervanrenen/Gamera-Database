#pragma once

#include <utility>

namespace dbi {

class HashMapBucketPage {
    public:    

        void initialize()
        {
            entryCount = 0;
        }

        /// If the key is already in use, the coresponding value is overwritten
        /// If space if left in the bucket, a key-value pair is appended
        /// Returns true is these two cases, false otherwise
        template<class Key, class Value>
        bool insert(Key key, Value value) {
            std::cout << "insert on bucket" << std::endl;
            return true;
            // for(std::size_t i = 0; i < entries.size(); i++) {
            //     // Key already in use -> overwrite
            //     if(entries[i].first == key) {
            //         entries[i].second = value;
            //         return true;
            //     }
            // }            
            // // Try to append
            // if(entries.size() < maxSize) {
            //     entries.push_back(std::make_pair(key,value));
            //     return true;
            // } else
            //     return false;
        }
        
        /// Returns the value associated to the provided key if present. Null otherwise.
        template<class Key, class Value>
        Value get(Key key) {
            return Value(0);
            // for(std::size_t i = 0; i < maxSize; i++) {
            //     if(entries[i].first == key)
            //         return entries[i].second;
            // }
            // return NULL;
        }
        
    private:
        uint16_t entryCount;
        static const uint16_t maxSize = 2;

        template<class Key, class Value>
        struct Entry {
            Key key;
            Value value;
        };
};

}