#pragma once

#include "util/Math.hpp"
#include <vector>
#include <iostream>
#include <cassert>
#include <mutex>

namespace util {

/// Features: lock free, offsets, expects unique keys, fast, no remove, max size, ONE writer, many readers
template<class Key, class Value, class SizeType = uint32_t>
class ConcurrentOffsetHash {
public:
   /// Constructor
   ConcurrentOffsetHash(SizeType size)
   : mask(util::nextPowerOfTwo(size)-1)
   , nextOffset(1)
   , entries(size+1)
   , offsets(util::nextPowerOfTwo(size))
   {
   }

   /// Add element with key and value to the map
   void insert(const Key& key, const Value& value) {
      // Get hash
      std::unique_lock<std::mutex> l(guard);
      SizeType hashVal = key&mask;
      SizeType entryOffset = nextOffset++;
      assert(entryOffset < entries.size());

      // Insert entry
      entries[entryOffset].key = key;
      entries[entryOffset].value = value;
      do {
         entries[entryOffset].next = offsets[hashVal];
      } while (!__sync_bool_compare_and_swap(&offsets[hashVal], entries[entryOffset].next, entryOffset));
   }

   /// Find element with given key, null if not present
   Value* find(Key key) {
      // Get hash
      std::unique_lock<std::mutex> l(guard);
      SizeType hashVal = key&mask;
      uint32_t pos = offsets[hashVal];

      // Try to find key == key
      for(; pos!=0; pos=entries[pos].next)
         if(entries[pos].key == key)
            return &entries[pos].value;

      // Otherwise not found
      return nullptr;
   }

   /// Update key of element with key equal current to next
   void updateKey(Key current, Key next) {
      // Get hash and lock
      std::unique_lock<std::mutex> l(guard);
      SizeType currentHashVal = current&mask;
      SizeType nextHashVal = next&mask;

      // Find current (*currentPos will contain elements offset)
      SizeType* currentPos = &offsets[currentHashVal];
      for(; *currentPos!=0; currentPos=&entries[*currentPos].next)
         if(entries[*currentPos].key == current)
            break;
      assert(*currentPos != 0);

      // No need to move element if
      if(currentHashVal == nextHashVal) {
         entries[*currentPos].key = next;
         return;
      }

      // Remove current
      SizeType currentElement = *currentPos;
      *currentPos = entries[*currentPos].next;

      // Insert at new location
      entries[currentElement].key = next;
      entries[currentElement].next = offsets[nextHashVal];
      offsets[nextHashVal] = currentElement;
   }

   ~ConcurrentOffsetHash() {

   }

private:
   struct Entry {
      Entry() {}
      Key key;
      Value value;
      SizeType next;
   };

   SizeType mask;
   SizeType nextOffset;
   std::vector<Entry> entries;
   std::vector<SizeType> offsets;
   std::mutex guard;
};

}
