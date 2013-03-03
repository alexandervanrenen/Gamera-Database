#include "util/Utility.hpp"
#include "gda/random_generator.hpp"
#include "ExternalSort.hpp"
#include <cstdio>

TEST(ExternalSort, AlreadySorted) {
  EXPECT_TRUE(dbi_utility::createTestFile("bin/externalsortinput", 1<<10, [](uint64_t i){return i;}));
  dbi::ExternalSort sorty;
  sorty.simpleSort("bin/externalsortinput", "bin/externalsortoutput");

  bool check = true;
  uint64_t i = 0;
  EXPECT_TRUE(dbi_utility::foreachInFile("bin/externalsortoutput", [&](uint64_t data){check&=data==i++;}));
  EXPECT_TRUE(check);
  EXPECT_EQ(i , 1<<10);
  remove("bin/externalsortinput");
  remove("bin/externalsortoutput");
}

TEST(ExternalSort, Simple) {
  gda::RandomGenerator ranny;
  EXPECT_TRUE(dbi_utility::createTestFile("bin/externalsortinput", 1<<10, [&](uint64_t i){return ranny.rand();}));
  dbi::ExternalSort sorty;
  sorty.simpleSort("bin/externalsortinput", "bin/externalsortoutput");

  uint64_t last = 0;
  bool check = true;
  uint64_t i = 0;
  EXPECT_TRUE(dbi_utility::foreachInFile("bin/externalsortoutput", [&](uint64_t data){check&=last<=data; last=data; i++;}));
  EXPECT_TRUE(check);
  EXPECT_EQ(i , 1<<10);
  remove("bin/externalsortinput");
  remove("bin/externalsortoutput");
}

TEST(ExternalSort, Complex) {
  gda::RandomGenerator ranny;
  EXPECT_TRUE(dbi_utility::createTestFile("bin/externalsortinput", 1<<10, [&](uint64_t i){return ranny.rand();}));
  dbi::ExternalSort sorty;
  sorty.complexSort("bin/externalsortinput", "bin/externalsortoutput", 1024);

  uint64_t last = 0;
  bool check = true;
  uint64_t i = 0;
  EXPECT_TRUE(dbi_utility::foreachInFile("bin/externalsortoutput", [&](uint64_t data){check&=last<=data; last=data; i++;}));
  EXPECT_TRUE(check);
  EXPECT_EQ(i , 1<<10);
  remove("bin/externalsortinput");
  remove("bin/externalsortoutput");
}
