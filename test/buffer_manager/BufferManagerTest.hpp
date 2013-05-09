#pragma once

#include "gtest/gtest.h"
#include "FunkesBufferManagerTest.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "buffer_manager/BufferFrame.hpp"
#include "common/Config.hpp"
#include "util/Utility.hpp"
#include <array>
#include <fstream>

TEST(BufferManager, Simple) {
   const std::string fileName = "swap_file";
   ASSERT_TRUE(dbi::util::createFile(fileName, dbi::kPageSize*1024));

   dbi::BufferManager bm("swap_file", 10);
   dbi::BufferFrame& bf0 = bm.fixPage(0, dbi::kExclusive);
   dbi::BufferFrame& bf1 = bm.fixPage(1, dbi::kShared);
   bm.unfixPage(bf0, true);
   bm.unfixPage(bf1, false);

   remove("swap_file");
}

TEST(BufferManager, FunkeTest) {
   int argc = 5;
   std::array<const char*, 5> argv = {{"", "swap_file", "1024", "64", "2"}};

   const std::string fileName = "swap_file";

   ASSERT_TRUE(dbi::util::createFile(fileName, dbi::kPageSize*1024));
   ASSERT_EQ(main_funke(argc, const_cast<char**>(&std::get<0>(argv))),0);
   remove("swap_file");
}
