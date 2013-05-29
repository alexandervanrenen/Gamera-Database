#pragma once

#include <utility>

namespace dbi {

template<typename Key, typename Value>
class Bucket {
    
    public:
        Bucket() {
            
        }   
        
        ~Bucket() {
            
        }
        
        /// If the key is already in use, the coresponding value is overwritten
        /// If space if left in the bucket, a key-value pair is appended
        /// Returns true is these two cases, false otherwise
        bool insert(Key key, Value value) {
            for(std::size_t i = 0; i < entries.size(); i++) {
                // Key already in use -> overwrite
                if(entries[i].first == key) {
                    entries[i].second = value;
                    return true;
                }
            }            
            // Try to append
            if(entries.size() < maxSize) {
                entries.push_back(std::make_pair(key,value));
                return true;
            } else
                return false;
        }
        
        /// Returns the value associated to the provided key if present. Null otherwise.
        Value get(Key key) {
            for(std::size_t i = 0; i < maxSize; i++) {
                if(entries[i].first == key)
                    return entries[i].second;
            }
            return NULL;
        }
        
    private:
        static const uint16_t maxSize = 2;
        std::vector<std::pair<Key,Value>> entries;
        
    
            
    
};

}