#include "util/Utility.hpp"

TEST(Utility, FileCreatorSimple) {
   EXPECT_TRUE(dbiu::createTestFile("bin/testdata", 1<<10, [](uint64_t i) {return i;}));
   bool check = true;
   uint64_t i = 0;
   EXPECT_TRUE(dbiu::foreachInFile("bin/testdata", [&](uint64_t data) {check&=data==i++;}));
   EXPECT_TRUE(check);
   EXPECT_EQ(i , 1<<10);
   remove("bin/testdata");
}

TEST(Utility, FileCreatorBorders) {
   {
      EXPECT_TRUE(dbiu::createTestFile("bin/testdata", 1<<10 + 1, [](uint64_t i) {return i;}));
      bool check = true;
      double i = 0;
      EXPECT_TRUE(dbiu::foreachInFile("bin/testdata", [&](double data) {check&=data==i++;}));
      EXPECT_TRUE(check);
      EXPECT_EQ(i , 1<<10 + 1);
      remove("bin/testdata");
   }

   {
      EXPECT_TRUE(dbiu::createTestFile("bin/testdata", 1<<10 - 1, [](uint64_t i) {return i;}));
      bool check = true;
      uint64_t i = 0;
      EXPECT_TRUE(dbiu::foreachInFile("bin/testdata", [&](double data) {check&=data==i++;}));
      EXPECT_TRUE(check);
      EXPECT_EQ(i , 1<<10 - 1);
      remove("bin/testdata");
   }

   {
      EXPECT_TRUE(dbiu::createTestFile("bin/testdata", 1, [](uint64_t i) {return i;}));
      bool check = true;
      uint64_t i = 0;
      EXPECT_TRUE(dbiu::foreachInFile("bin/testdata", [&](double data) {check&=data==i++;}));
      EXPECT_TRUE(check);
      EXPECT_EQ(i , 1);
      remove("bin/testdata");
   }

   {
      EXPECT_TRUE(dbiu::createTestFile("bin/testdata", 0, [](uint64_t i) {return i;}));
      bool check = true;
      uint64_t i = 0;
      EXPECT_TRUE(dbiu::foreachInFile("bin/testdata", [&](double data) {check&=data==i++;}));
      EXPECT_TRUE(check);
      EXPECT_EQ(i , 0);
      remove("bin/testdata");
   }
}
