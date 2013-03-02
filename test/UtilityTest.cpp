#include "util/Utility.hpp"

TEST(Utility, FileCreatorSimple) {
  EXPECT_TRUE(dbi_utility::createTestFile<uint64_t>("test/testdata", 1<<10, [](uint64_t i){return i;}));
  bool check = true;
  uint64_t i = 0;
  EXPECT_TRUE(dbi_utility::readTestFile<uint64_t>("test/testdata", [&](uint64_t data){check&=data==i++;}));
  EXPECT_TRUE(check);
  EXPECT_EQ(i , 1<<10);
}

TEST(Utility, FileCreatorBorders) {
  {
    EXPECT_TRUE(dbi_utility::createTestFile<double>("test/testdata", 1<<10 + 1, [](uint64_t i){return static_cast<double>(i);}));
    bool check = true;
    double i = 0;
    EXPECT_TRUE(dbi_utility::readTestFile<double>("test/testdata", [&](double data){check&=data==i++;}));
    EXPECT_TRUE(check);
    EXPECT_EQ(i , 1<<10 + 1);
  }

  {
    EXPECT_TRUE(dbi_utility::createTestFile<double>("test/testdata", 1<<10 - 1, [](uint64_t i){return static_cast<double>(i);}));
    bool check = true;
    uint64_t i = 0;
    EXPECT_TRUE(dbi_utility::readTestFile<double>("test/testdata", [&](double data){check&=data==i++;}));
    EXPECT_TRUE(check);
    EXPECT_EQ(i , 1<<10 - 1);
  }

  {
    EXPECT_TRUE(dbi_utility::createTestFile<double>("test/testdata", 1, [](uint64_t i){return static_cast<double>(i);}));
    bool check = true;
    uint64_t i = 0;
    EXPECT_TRUE(dbi_utility::readTestFile<double>("test/testdata", [&](double data){check&=data==i++;}));
    EXPECT_TRUE(check);
    EXPECT_EQ(i , 1);
  }

  {
    EXPECT_TRUE(dbi_utility::createTestFile<double>("test/testdata", 0, [](uint64_t i){return static_cast<double>(i);}));
    bool check = true;
    uint64_t i = 0;
    EXPECT_TRUE(dbi_utility::readTestFile<double>("test/testdata", [&](double data){check&=data==i++;}));
    EXPECT_TRUE(check);
    EXPECT_EQ(i , 0);
  }
}
