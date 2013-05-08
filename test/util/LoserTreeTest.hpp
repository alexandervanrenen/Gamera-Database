
#include "gtest/gtest.h"
#include "util/LoserTree.hpp"
#include <limits>
#include <set>

TEST(LoserTree, Small) {
   // Build
   std::vector<uint32_t> data(4);
   data[0] = 3;
   data[1] = 1;
   data[2] = 6;
   data[3] = 2;
   LoserTree<uint32_t, 99> tree(data);

   // Change
   EXPECT_EQ(tree.min(), 1u); // 1,2,3,6
   EXPECT_EQ(tree.popAndReplace(9), 1u);
   EXPECT_EQ(tree.min(), 2u); // 2,3,6,9
   EXPECT_EQ(tree.popAndReplace(5), 2u);
   EXPECT_EQ(tree.min(), 3u); // 3,5,6,9
   EXPECT_EQ(tree.popAndReplace(2), 3u);
   EXPECT_EQ(tree.min(), 2u); // 2,5,6,9
   EXPECT_EQ(tree.popAndReplace(7), 2u);
   EXPECT_EQ(tree.min(), 5u); // 5,6,7,9
}

TEST(LoserTree, MinRandomized) {
   const uint32_t maxEntries = 4096;
   for(uint32_t run=0; run<100; run++) {
      // Build
      uint32_t entries = rand() % maxEntries;
      std::vector<uint32_t> data(maxEntries);
      std::multiset<uint32_t> reference;
      for(uint32_t e=0; e<entries; e++) {
         uint32_t next = rand() % maxEntries;
         reference.insert(next);
         data[e] = next;
      }
      for(uint32_t i=entries; i<maxEntries; i++) {
         data[i] = std::numeric_limits<uint32_t>::max();
         reference.insert(std::numeric_limits<uint32_t>::max());
      }
      LoserTree<uint32_t, std::numeric_limits<uint32_t>::max()> tree(data);

      // Change
      for(uint32_t i=0; i<500; i++) {
         uint32_t next = rand() % maxEntries;
         EXPECT_EQ(*reference.begin(), tree.min());
         EXPECT_EQ(*reference.begin(), tree.popAndReplace(next));
         reference.erase(reference.begin());
         reference.insert(next);
      }
   }
}
