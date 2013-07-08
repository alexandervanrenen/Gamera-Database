#include "gtest/gtest.h"
#include "test/TestConfig.hpp"
#include "util/Utility.hpp"

TEST(Utility, FileCreatorSimple)
{
   // Create small file
   EXPECT_TRUE(dbi::util::createTestFile("bin/testdata", 1 << 9, [](uint32_t i) {return i;}));
   EXPECT_TRUE(dbi::util::getFileLength("bin/testdata") == (1<<9)*sizeof(uint32_t));
   bool check = true;
   uint32_t i = 0;
   EXPECT_TRUE(dbi::util::foreachInFile("bin/testdata", [&](uint32_t data) {check&=data==i++;}));
   EXPECT_TRUE(check);
   EXPECT_EQ(i, 1ul << 9);

   // Create bigger one
   EXPECT_TRUE(dbi::util::createTestFile("bin/testdata", 1 << 10, [](uint32_t i) {return i;}));
   EXPECT_TRUE(dbi::util::getFileLength("bin/testdata") == (1<<10)*sizeof(uint32_t));
   check = true;
   i = 0;
   EXPECT_TRUE(dbi::util::foreachInFile("bin/testdata", [&](uint32_t data) {check&=data==i++;}));
   EXPECT_TRUE(check);
   EXPECT_EQ(i, 1ul << 10);

   remove("bin/testdata");
}

TEST(Utility, FileCreatorBorders)
{
   {
      EXPECT_TRUE(dbi::util::createTestFile("bin/testdata", (1 << 10) + 1, [](uint32_t i) {return i;}));
      bool check = true;
      uint32_t i = 0;
      EXPECT_TRUE(dbi::util::foreachInFile("bin/testdata", [&](uint32_t data) {check&=data==i++;}));
      EXPECT_TRUE(check);
      EXPECT_EQ(i, (1 << 10) + 1);
      remove("bin/testdata");
   }

   {
      EXPECT_TRUE(dbi::util::createTestFile("bin/testdata", (1 << 10) - 1, [](uint32_t i) {return i;}));
      bool check = true;
      uint32_t i = 0;
      EXPECT_TRUE(dbi::util::foreachInFile("bin/testdata", [&](uint32_t data) {check&=data==i++;}));
      EXPECT_TRUE(check);
      EXPECT_EQ(i, (1ul << 10) - 1);
      remove("bin/testdata");
   }

   {
      EXPECT_TRUE(dbi::util::createTestFile("bin/testdata", 1, [](uint32_t i) {return i;}));
      bool check = true;
      uint32_t i = 0;
      EXPECT_TRUE(dbi::util::foreachInFile("bin/testdata", [&](uint32_t data) {check&=data==i++;}));
      EXPECT_TRUE(check);
      EXPECT_EQ(i, 1ul);
      remove("bin/testdata");
   }

   {
      EXPECT_TRUE(dbi::util::createTestFile("bin/testdata", 0, [](uint32_t i) {return i;}));
      bool check = true;
      uint32_t i = 0;
      EXPECT_TRUE(dbi::util::foreachInFile("bin/testdata", [&](uint32_t data) {check&=data==i++;}));
      EXPECT_TRUE(check);
      EXPECT_EQ(i, 0ul);
      remove("bin/testdata");
   }
}
