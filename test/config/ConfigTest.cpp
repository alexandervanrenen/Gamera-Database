#include "common/Config.hpp"
#include "gtest/gtest.h"
#include "test/TestConfig.hpp"
#include <array>
#include <fstream>
#include <iostream>
#include <set>
#include <string>

using namespace std;
using namespace dbi;

TEST(Config, BasicCheck)
{
   ASSERT_TRUE(kPageSize == (1<<12) || kPageSize == (1<<13) || kPageSize == (1<<14) || kPageSize == (1<<15));
}
