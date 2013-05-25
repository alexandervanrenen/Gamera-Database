#include "util/ExtentStore.hpp"
#include "util/Extent.hpp"
#include "gtest/gtest.h"
#include <array>
#include <fstream>
#include <string>
#include <set>
#include <iostream>

using namespace std;
using namespace dbi;
using namespace dbi::util;

TEST(SegmentManager, ExtentStore)
{
   ExtentStore extentStore;

   // Add
   extentStore.add(Extent(5, 10));
   ASSERT_ANY_THROW(extentStore.add(Extent(5, 6)));
   ASSERT_ANY_THROW(extentStore.add(Extent(7, 8)));
   ASSERT_ANY_THROW(extentStore.add(Extent(9, 10)));
   ASSERT_EQ(extentStore.numPages(), 5ull);
   extentStore.add(Extent(15, 20));
   ASSERT_EQ(extentStore.numPages(), 10ull);
   extentStore.add(Extent(11, 15));
   ASSERT_EQ(extentStore.numPages(), 14ull);
   extentStore.add(Extent(10, 11));
   ASSERT_EQ(extentStore.numPages(), 15ull);
   ASSERT_EQ(extentStore.get().size(), 1ull);
   ASSERT_EQ(extentStore.get()[0], Extent(5,20));

   // Remove
   ASSERT_ANY_THROW(extentStore.remove(Extent(4, 5)));
   ASSERT_ANY_THROW(extentStore.remove(Extent(20, 21)));
   ASSERT_ANY_THROW(extentStore.remove(Extent(1, 2)));
   extentStore.remove(Extent(5,6));
   extentStore.remove(Extent(19,20));
   extentStore.remove(Extent(9,11));
   extentStore.remove(Extent(6,9));
   ASSERT_EQ(extentStore.get().size(), 1ull);
   ASSERT_EQ(extentStore.get()[1], Extent(11,19));
}
