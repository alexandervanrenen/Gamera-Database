#pragma once

#include "FunkeSlottedTest.hpp"
#include "util/Utility.hpp"
#include "common/Config.hpp"
#include "gtest/gtest.h"
#include <array>
#include <fstream>

TEST(SegmentManager, FunkeTest) {

   const std::string fileName = "swap_file";
   const uint32_t pages = 10*1000;

   ASSERT_TRUE(dbiu::createFile(fileName, pages*dbi::kPageSize));
   ASSERT_EQ(run(fileName, pages), 0);
   remove(fileName.c_str());
}
