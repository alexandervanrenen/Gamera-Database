#include "gtest/gtest.h"
#include "common/Config.hpp"
#include <array>
#include <fstream>
#include <string>
#include <set>
#include <iostream>

using namespace std;
using namespace dbi;

TEST(Config, BasicCheck)
{
   ASSERT_TRUE(kPageSize == (1<<12) || kPageSize == (1<<13) || kPageSize == (1<<14) || kPageSize == (1<<15));
}
