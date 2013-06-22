#include "gtest/gtest.h"
#include "segment_manager/CompactExtentStore.hpp"
#include "segment_manager/Extent.hpp"
#include "test/TestConfig.hpp"
#include <array>
#include <fstream>
#include <iostream>
#include <set>
#include <string>

using namespace std;
using namespace dbi;

TEST(SegmentManager, CompactExtentStore)
{
   CompactExtentStore extentStore;

   // Add
   extentStore.add(Extent(PageId(5), PageId(10)));
   ASSERT_ANY_THROW(extentStore.add(Extent(PageId(5), PageId(6))));
   ASSERT_ANY_THROW(extentStore.add(Extent(PageId(7), PageId(8))));
   ASSERT_ANY_THROW(extentStore.add(Extent(PageId(9), PageId(10))));
   ASSERT_EQ(extentStore.numPages(), 5ull);
   extentStore.add(Extent(PageId(15), PageId(20)));
   ASSERT_EQ(extentStore.numPages(), 10ull);
   extentStore.add(Extent(PageId(11), PageId(15)));
   ASSERT_EQ(extentStore.numPages(), 14ull);
   extentStore.add(Extent(PageId(10), PageId(11)));
   ASSERT_EQ(extentStore.numPages(), 15ull);
   ASSERT_EQ(extentStore.get().size(), 1ull);
   ASSERT_EQ(extentStore.get()[0], Extent(PageId(5), PageId(20)));

   // Remove
   ASSERT_ANY_THROW(extentStore.remove(Extent(PageId(4), PageId(5))));
   ASSERT_ANY_THROW(extentStore.remove(Extent(PageId(20), PageId(21))));
   ASSERT_ANY_THROW(extentStore.remove(Extent(PageId(1), PageId(2))));
   extentStore.remove(Extent(PageId(5),PageId(6)));
   extentStore.remove(Extent(PageId(19),PageId(20)));
   extentStore.remove(Extent(PageId(9),PageId(11)));
   extentStore.remove(Extent(PageId(6),PageId(9)));
   ASSERT_EQ(extentStore.numPages(), 8ull);
   ASSERT_EQ(extentStore.get().size(), 1ull);
   ASSERT_EQ(extentStore.get()[1], Extent(PageId(11),PageId(19)));
}
