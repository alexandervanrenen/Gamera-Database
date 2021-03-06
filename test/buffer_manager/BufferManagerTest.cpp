#include "buffer_manager/BufferFrame.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "common/Config.hpp"
#include "FunkesBufferManagerTest.hpp"
#include "gtest/gtest.h"
#include "test/TestConfig.hpp"
#include "util/Utility.hpp"
#include <array>
#include <fstream>

TEST(BufferManager, Simple)
{
   uint32_t pages = 1000;
   assert(kSwapFilePages>=pages);

   dbi::BufferManager bm(kSwapFileName, 10);
   dbi::BufferFrame& bf0 = bm.fixPage(dbi::PageId(0), dbi::kExclusive);
   dbi::BufferFrame& bf1 = bm.fixPage(dbi::PageId(1), dbi::kShared);
   bm.unfixPage(bf0, true);
   bm.unfixPage(bf1, false);
}

TEST(BufferManager, FunkeTest)
{
   int argc = 5;
   uint32_t pages = 1000;
   assert(kSwapFilePages>=pages);
   std::array<const char*, 5> argv = { {"", kSwapFileName.c_str(), "1000", "64", "2"}};
   ASSERT_EQ(main_funke(argc, const_cast<char**>(&std::get<0>(argv))), 0);
}
