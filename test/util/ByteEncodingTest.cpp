#include "gtest/gtest.h"
#include "util/ByteEncoding.hpp"
#include "test/TestConfig.hpp"

TEST(ByteEncoding, Check)
{
   for(uint32_t i=0; i<dbi::kPageSize; i++)
      ASSERT_TRUE(i >= dbi::util::decodeBytes(dbi::util::encodeBytes(i)));
}
