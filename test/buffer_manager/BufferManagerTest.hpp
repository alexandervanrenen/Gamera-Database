#pragma once

#include "gtest/gtest.h"
#include "FunkesBufferManagerTest.hpp"
#include "common/Config.hpp"
#include "util/Utility.hpp"
#include <array>
#include <fstream>

TEST(BufferManager, FunkeTest) {
   int argc = 5;
   std::array<const char*, 5> argv = {{"", "swap_file", "1024", "64", "2"}};

   const std::string fileName = "swap_file";

   ASSERT_TRUE(dbiu::createFile(fileName, dbi::kPageSize*1024));
   ASSERT_EQ(main_funke(argc, const_cast<char**>(&std::get<0>(argv))),0);
}
