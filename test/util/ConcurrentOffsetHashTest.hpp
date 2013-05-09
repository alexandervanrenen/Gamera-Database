#include "gtest/gtest.h"
#include "util/ConcurrentOffsetHash.hpp"
#include <iostream>
#include <unordered_map>

TEST(ConcurrentOffsetHash, Small) {
   {
      dbi::util::ConcurrentOffsetHash<uint32_t, uint32_t> hashmap(16);
      hashmap.insert(2) = 123;
      hashmap.insert(3) = 456;
      ASSERT_TRUE(hashmap.find(1) == nullptr);
      ASSERT_TRUE(hashmap.find(2) != nullptr && *hashmap.find(2) == 123);
      ASSERT_TRUE(hashmap.find(3) != nullptr && *hashmap.find(3) == 456);
      ASSERT_TRUE(hashmap.find(4) == nullptr);
      hashmap.updateKey(3,4);
      ASSERT_TRUE(hashmap.find(3) == nullptr);
      ASSERT_TRUE(hashmap.find(4) != nullptr && *hashmap.find(4) == 456);
   }
   {
      dbi::util::ConcurrentOffsetHash<uint32_t, uint32_t> hashmap(8);
      hashmap.insert(1) = 123;
      hashmap.insert(9) = 456;
      hashmap.insert(17) = 789;
      hashmap.updateKey(9, 2);
      ASSERT_TRUE(hashmap.find(9) == nullptr);
      ASSERT_TRUE(hashmap.find(2) != nullptr && *hashmap.find(2) == 456);
      ASSERT_TRUE(hashmap.find(1) != nullptr && *hashmap.find(1) == 123);
      ASSERT_TRUE(hashmap.find(17) != nullptr && *hashmap.find(17) == 789);
   }
}

template<class T>
bool doInsert(uint32_t opertationCount, T& hashmap, std::unordered_map<uint64_t, uint64_t>& reference) {
   // Fill both maps
   for(uint64_t i=0; i<opertationCount; i++) {
      uint64_t key = rand();
      uint64_t value = rand();
      if(reference.count(key) == 0) {
         hashmap.insert(key) = value;
         reference.insert(std::make_pair(key, value));
      }
   }

   // Check if content is equal
   for(auto iter : reference) {
      uint64_t* value = hashmap.find(iter.first);
      if(value==nullptr || iter.second != *value)
         return false;
   }
   return true;
}

template<class T>
bool doKeyUpdates(uint32_t opertationCount, T& hashmap, std::unordered_map<uint64_t, uint64_t>& reference) {
   // Move stuff around
   for(uint32_t i=0; i<opertationCount; i++) {
      uint64_t ele = rand() % reference.size();
      uint64_t nextKey = rand();
      auto iter = reference.begin();
      for(; ele!=0; ele--)
         iter++;

      uint32_t currentKey = iter->first;
      uint32_t val = iter->second;
      reference.erase(iter);
      reference.insert(std::make_pair(nextKey, val));
      hashmap.updateKey(currentKey, nextKey);
   }

   // Check if content is equal
   for(auto iter : reference) {
      uint64_t* value = hashmap.find(iter.first);
      if(value==nullptr || iter.second != *value)
         return false;
   }
   return true;
}

TEST(ConcurrentOffsetHash, SingleThreadedInsert) {
   for(uint32_t i=0; i<100; i++) {
      dbi::util::ConcurrentOffsetHash<uint64_t, uint64_t> hashmap(128);
      std::unordered_map<uint64_t, uint64_t> reference;
      ASSERT_TRUE(doInsert(128, hashmap, reference));
   }
}

TEST(ConcurrentOffsetHash, SingleThreadedMove) {
   for(uint32_t i=0; i<100; i++) {
      dbi::util::ConcurrentOffsetHash<uint64_t, uint64_t> hashmap(128);
      std::unordered_map<uint64_t, uint64_t> reference;
      ASSERT_TRUE(doInsert(128, hashmap, reference));
      ASSERT_TRUE(doKeyUpdates(128, hashmap, reference));
   }
}


