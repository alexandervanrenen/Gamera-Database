#include "util/Utility.hpp"
#include "gda/random_generator.hpp"
#include "ExternalSort.hpp"

TEST(ExternalSort, AlreadySorted) {
  EXPECT_TRUE(dbi_utility::createTestFile<uint64_t>("test/externalsortinput", 1<<10, [](uint64_t i){return i;}));
  dbi::ExternalSort sorty("test/externalsortinput");
  sorty.sort();

  bool check = true;
  uint64_t i = 0;
  EXPECT_TRUE(dbi_utility::readTestFile<uint64_t>("test/externalsortinput", [&](uint64_t data){check&=data==i++;}));
  EXPECT_TRUE(check);
  EXPECT_EQ(i , 1<<10);
}

TEST(ExternalSort, Simple) {
  gda::RandomGenerator ranny;
  EXPECT_TRUE(dbi_utility::createTestFile<uint64_t>("test/externalsortinput", 1<<10, [&](uint64_t i){return ranny.rand();}));
  dbi::ExternalSort sorty("test/externalsortinput");
  sorty.sort();

  uint64_t last = 0;
  bool check = true;
  uint64_t i = 0;
  EXPECT_TRUE(dbi_utility::readTestFile<uint64_t>("test/externalsortinput", [&](uint64_t data){check&=last<=data; last=data; i++;}));
  EXPECT_TRUE(check);
  EXPECT_EQ(i , 1<<10);
}
