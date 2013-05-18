#include <gtest/gtest.h>
#include <string>
#include <cstdint>
#include <cassert>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <string.h>

#include "btree/BTree.hpp"



/* Comparator functor for uint64_t*/
struct MyCustomUInt64Cmp {
   bool operator()(uint64_t a, uint64_t b) const {
      return a<b;
   }
};

template <unsigned len>
struct Char {
   char data[len];
};

/* Comparator functor for char */
template <unsigned len>
struct MyCustomCharCmp {
   bool operator()(const Char<len>& a, const Char<len>& b) const {
      return memcmp(a.data, b.data, len) < 0;
   }
};

typedef std::pair<uint32_t, uint32_t> IntPair;

/* Comparator for IntPair */
struct MyCustomIntPairCmp {
   bool operator()(const IntPair& a, const IntPair& b) const {
      if (a.first < b.first)
         return true;
      else
         return (a.first == b.first) && (a.second < b.second);
   }
};

template <class T>
const T& getKey(const uint64_t& i);

template <>
const uint64_t& getKey(const uint64_t& i) { return i; }

std::vector<std::string> char20;
template <>
const Char<20>& getKey(const uint64_t& i) {
   std::stringstream ss;
   ss << i;
   std::string s(ss.str());
   char20.push_back(std::string(20-s.size(), '0')+s);
   return *reinterpret_cast<const Char<20>*>(char20.back().data());
}

std::vector<IntPair> intPairs;
template <>
const IntPair& getKey(const uint64_t& i) {
   intPairs.push_back(std::make_pair(i/3, 3-(i%3)));
   return intPairs.back();
}


template <class T, class CMP>
void test(uint64_t n) {
    typedef dbi::TID TID;
   // Set up stuff, you probably have to change something here to match to your interfaces
   //BufferManager bm("/tmp/db", 10ul*1024ul*1024ul); // bogus arguments
   //SegmentManager sm(bm);
   //SegmentID spId = sm.createSegment(Segment::SegmentType::BTree);
   //BTreeSegment& seg = static_cast<BTreeSegment&>(sm.getSegment(spId));
   //BTree<T, CMP> bTree(seg);
    dbi::BTree<T, CMP> bTree;
    //std::cout << "LeafnodeSize: " << bTree.getLeafNodeSize() << std::endl;
   // Insert values
    TID tid;
    for (uint64_t i=0; i<n; ++i) {
        ASSERT_TRUE(bTree.insert(getKey<T>(i),static_cast<TID>(i*i)));
    }
    ASSERT_EQ(bTree.size(), n);
   // Check if they can be retrieved
    for (uint64_t i=0; i<n; ++i) {
      TID tid;
      ASSERT_TRUE(bTree.lookup(getKey<T>(i),tid));
      ASSERT_EQ(tid, i*i);
   }
   
   // Delete some values
   for (uint64_t i=0; i<n; ++i)
      if ((i%7)==0)
         ASSERT_TRUE(bTree.erase(getKey<T>(i)));
   // Check if the right ones have been deleted
   for (uint64_t i=0; i<n; ++i) {
      TID tid;
      if ((i%7)==0) {
         ASSERT_FALSE(bTree.lookup(getKey<T>(i),tid));
      } else {
         ASSERT_TRUE(bTree.lookup(getKey<T>(i),tid));
         ASSERT_EQ(tid, i*i);
      }
   }

   // Delete everything
   for (uint64_t i=0; i<n; ++i)
      bTree.erase(getKey<T>(i));
   ASSERT_EQ(bTree.size(), (uint64_t)0);
}

const uint64_t n = 1000*1000ul;

TEST(BTreeTest, FunkeTestUintKey) {
   // Test index with 64bit unsigned integers
   test<uint64_t, MyCustomUInt64Cmp>(n);
}

TEST(BTreeTest, FunkeTestCharKey) {
   // Test index with 20 character strings
   test<Char<20>, MyCustomCharCmp<20>>(n);
}

TEST(BTreeTest, FunkeTestCompoundKey) {
   // Test index with compound key
   test<IntPair, MyCustomIntPairCmp>(n);

}




TEST(BTreeTest, InitTest) {
    dbi::BTree<uint64_t, std::less<uint64_t>> tree;
    dbi::TID tid;
    //EXPECT_TRUE(tree.insert((uint64_t)1, 1));
    //EXPECT_TRUE(tree.lookup((uint64_t)1, tid));
    //EXPECT_EQ(tid, (uint64_t)1);
    EXPECT_FALSE(tree.lookup((uint64_t)2, tid));
    //for (uint64_t i=1; i <= tree.getInnerNodeSize() * tree.getLeafNodeSize(); i++) {
    //for (uint64_t i=tree.getInnerNodeSize() * tree.getLeafNodeSize(); i > 0; i--) {
    for (uint64_t i=1; i <= tree.getLeafNodeSize()+2; i++) {
        EXPECT_TRUE(tree.insert(i, i));
    }
    //auto it = tree.lookupRange(1, tree.getInnerNodeSize() * tree.getLeafNodeSize());
    auto it = tree.lookupRange(1, tree.getLeafNodeSize()+2);
    uint64_t key;
    //uint64_t oldkey = 0;
    for (uint64_t i=1; i <= tree.getLeafNodeSize()+2; i++) {
        EXPECT_TRUE(it->value(key, tid));
        //std::cout << "Key: " << key << ", TID: " << tid << std::endl;
        it->next();
    }
    EXPECT_FALSE(it->value(key, tid));
    //tree.visualize();
    /* 
    while (it->value(key, tid)) {
        std::cout << "In while\n";
        EXPECT_TRUE(key == oldkey+1);
        EXPECT_TRUE(tid == key);
        it->next();
    }
    */
    //EXPECT_TRUE(tree.insert(tree.getLeafNodeSize(), 1));
}

/*
int main(int argc, char **argv) {
    ::testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
*/
