#include "gtest/gtest.h"
#include "util/Utility.hpp"

TEST(Utility, FileCreatorSimple)
{
   // Create small file
   EXPECT_TRUE(dbi::util::createTestFile("bin/testdata", 1 << 9, [](uint64_t i) {return i;}));
   EXPECT_TRUE(dbi::util::getFileLength("bin/testdata") == (1<<9)*sizeof(uint64_t));
   bool check = true;
   uint64_t i = 0;
   EXPECT_TRUE(dbi::util::foreachInFile("bin/testdata", [&](uint64_t data) {check&=data==i++;}));
   EXPECT_TRUE(check);
   EXPECT_EQ(i, 1ul << 9);

   // Create bigger one
   EXPECT_TRUE(dbi::util::createTestFile("bin/testdata", 1 << 10, [](uint64_t i) {return i;}));
   EXPECT_TRUE(dbi::util::getFileLength("bin/testdata") == (1<<10)*sizeof(uint64_t));
   check = true;
   i = 0;
   EXPECT_TRUE(dbi::util::foreachInFile("bin/testdata", [&](uint64_t data) {check&=data==i++;}));
   EXPECT_TRUE(check);
   EXPECT_EQ(i, 1ul << 10);

   remove("bin/testdata");
}

TEST(Utility, FileCreatorBorders)
{
   {
      EXPECT_TRUE(dbi::util::createTestFile("bin/testdata", (1 << 10) + 1, [](uint64_t i) {return i;}));
      bool check = true;
      double i = 0;
      EXPECT_TRUE(dbi::util::foreachInFile("bin/testdata", [&](double data) {check&=data==i++;}));
      EXPECT_TRUE(check);
      EXPECT_EQ(i, (1 << 10) + 1);
      remove("bin/testdata");
   }

   {
      EXPECT_TRUE(dbi::util::createTestFile("bin/testdata", (1 << 10) - 1, [](uint64_t i) {return i;}));
      bool check = true;
      uint64_t i = 0;
      EXPECT_TRUE(dbi::util::foreachInFile("bin/testdata", [&](double data) {check&=data==i++;}));
      EXPECT_TRUE(check);
      EXPECT_EQ(i, (1ul << 10) - 1);
      remove("bin/testdata");
   }

   {
      EXPECT_TRUE(dbi::util::createTestFile("bin/testdata", 1, [](uint64_t i) {return i;}));
      bool check = true;
      uint64_t i = 0;
      EXPECT_TRUE(dbi::util::foreachInFile("bin/testdata", [&](double data) {check&=data==i++;}));
      EXPECT_TRUE(check);
      EXPECT_EQ(i, 1ul);
      remove("bin/testdata");
   }

   {
      EXPECT_TRUE(dbi::util::createTestFile("bin/testdata", 0, [](uint64_t i) {return i;}));
      bool check = true;
      uint64_t i = 0;
      EXPECT_TRUE(dbi::util::foreachInFile("bin/testdata", [&](double data) {check&=data==i++;}));
      EXPECT_TRUE(check);
      EXPECT_EQ(i, 0ul);
      remove("bin/testdata");
   }
}
