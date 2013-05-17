#pragma once

#include "util/Math.hpp"
#include "util/SpinLock.hpp"
#include <vector>
#include <iostream>
#include <cassert>
#include <limits>

namespace dbi {

namespace util {

/// Features: offsets, expects unique keys, no remove, max size, Allows for fuzzy reading
template<class Key, class Value, class SizeType = uint32_t>
class ConcurrentOffsetHash {
public:
   /// Constructor
   ConcurrentOffsetHash(SizeType size)
   : mask((util::nextPowerOfTwo(size) << 2) - 1)
   , invalid(std::numeric_limits<SizeType>::max())
   , nextOffset(0)
   , entries(size)
   , offsets(util::nextPowerOfTwo(size) << 2, invalid)
   , locks(util::nextPowerOfTwo(size) << 2) // TODO: Smaller locks then offsets vector
   {
      assert(invalid > entries.size());
      for(auto& iter : entries)
         iter.next = invalid;
   }

   /// Add element with key and value to the map
   Value& insert(const Key& key)
   {
      // Get hash
      SizeType hashVal = key & mask;
      SizeType entryOffset = nextOffset++;
      assert(entryOffset < entries.size());

      // Insert entry
      entries[entryOffset].key = key;
      entries[entryOffset].next = offsets[hashVal];
      offsets[hashVal] = entryOffset;
      return entries[entryOffset].value;
   }

   /// Find element with given key, null if not present
   Value* fuzzyFind(Key key)
   {
      // Get hash
      SizeType hashVal = key & mask;
      uint32_t pos = offsets[hashVal];

      // Try to find key == key
      for(; pos != invalid; pos = entries[pos].next)
         if(entries[pos].key == key)
            return &entries[pos].value;

      // Otherwise not found
      return nullptr;
   }

   /// Find element with given key, null if not present
   Value* find(Key key)
   {
      // Get hash
      SizeType hashVal = key & mask;
      locks[hashVal].lock();
      uint32_t pos = offsets[hashVal];

      // Try to find key == key
      for(; pos != invalid; pos = entries[pos].next)
         if(entries[pos].key == key) {
            Value* result = &entries[pos].value;
            locks[hashVal].unlock();
            return result;
         }

      // Otherwise not found
      locks[hashVal].unlock();
      return nullptr;
   }

   /// Update key of element with key equal current to next
   void updateKey(Key current, Key next)
   {
      // Get hash and lock
      SizeType currentHashVal = current & mask;
      SizeType nextHashVal = next & mask;

      // Find current (*currentPos will contain elements offset)
      locks[currentHashVal].lock();
      SizeType* currentPos = &offsets[currentHashVal];
      for(; *currentPos != invalid; currentPos = &entries[*currentPos].next)
         if(entries[*currentPos].key == current)
            break;
      assert(*currentPos != invalid);

      // No need to move element if
      if(currentHashVal == nextHashVal) {
         entries[*currentPos].key = next;
         locks[currentHashVal].unlock();
         return;
      }

      // Remove current
      SizeType currentElement = *currentPos;
      *currentPos = entries[*currentPos].next;
      locks[currentHashVal].unlock();

      // Insert at new location
      locks[nextHashVal].lock();
      entries[currentElement].key = next;
      entries[currentElement].next = offsets[nextHashVal];
      offsets[nextHashVal] = currentElement;
      locks[nextHashVal].unlock();
   }

   struct Entry {
      Entry()
      {
      }
      Key key;
      Value value;
      SizeType next;
   };

   std::vector<Entry>& data()
   {
      return entries;
   }

private:

   const SizeType mask;
   const SizeType invalid;
   SizeType nextOffset;
   std::vector<Entry> entries;
   std::vector<SizeType> offsets;
   std::vector<util::SpinLock> locks;
};

}

}
