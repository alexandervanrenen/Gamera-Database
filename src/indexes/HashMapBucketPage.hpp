#pragma once

#include <iostream>
#include <utility>

namespace dbi {

template<class Key, class Value>
class HashMapBucketPage {
public:
   /// Set up this bucket
   void initialize()
   {
      header.entryCount = 0;
   }

   /// Check if the given key is contained in this bucket
   bool has(Key key)
   {
      // Update if value exists
      for(uint32_t i=0; i<header.entryCount; i++)
         if(entries[i].key == key)
            return true;
   }

   /// Assumes uniqueness of the keys.
   /// If space if left in the bucket, a key-value pair is appended and true is returned. Otherwise false.
   bool insert(Key key, Value value)
   {
      assert(!has(key));

      // Try to append
      if(header.entryCount < maxSize) {
         entries[header.entryCount] = Entry{key, value};
         header.entryCount++;
         return true;
      }

      return false;
   }

   /// Returns the value associated to the provided key if present. Null otherwise.
   Value* get(Key key) {
      for(uint32_t i=0; i<header.entryCount; i++)
         if(entries[i].key == key)
            return &entries[i].value;
   }

   /// Print info about this bucket into the given stream
   void dump(std::ostream& os)
   {
      os << "entry count: " << header.entryCount << std::endl;
      for(uint32_t i=0; i<header.entryCount; i++)
         os << i << " : " << entries[i].key << " " << entries[i].value << std::endl;
   }

private:
   static const uint16_t maxSize = 2;

   struct {
      uint16_t entryCount;
   } header;

   struct Entry {
      Key key;
      Value value;
   };

   std::array<Entry, (kPageSize-sizeof(header) / sizeof(Entry))> entries;
};

}
