#include "util/Utility.hpp"
#include "ExternalSort.hpp"
#include <cstdio>

TEST(ExternalSort, AlreadySorted) {
   EXPECT_TRUE(dbiu::createTestFile("bin/externalsortinput", 1<<10, [](uint64_t i) {return i;}));
   dbi::ExternalSort sorty;
   sorty.simpleSort("bin/externalsortinput", "bin/externalsortoutput");

   bool check = true;
   uint64_t i = 0;
   EXPECT_TRUE(dbiu::foreachInFile("bin/externalsortoutput", [&](uint64_t data) {check&=data==i++;}));
   EXPECT_TRUE(check);
   EXPECT_EQ(i , 1<<10);
   remove("bin/externalsortinput");
   remove("bin/externalsortoutput");
}

TEST(ExternalSort, Simple) {
   EXPECT_TRUE(dbiu::createTestFile("bin/externalsortinput", 1<<10, [&](uint64_t i) {return rand();}));
   dbi::ExternalSort sorty;
   sorty.simpleSort("bin/externalsortinput", "bin/externalsortoutput");

   uint64_t last = 0;
   bool check = true;
   uint64_t i = 0;
   EXPECT_TRUE(dbiu::foreachInFile("bin/externalsortoutput", [&](uint64_t data) {check&=last<=data; last=data; i++;}));
   EXPECT_TRUE(check);
   EXPECT_EQ(i , 1<<10);
   remove("bin/externalsortinput");
   remove("bin/externalsortoutput");
}

TEST(ExternalSort, ComplexSmall) {
   EXPECT_TRUE(dbiu::createTestFile("bin/data", 1<<10, [&](uint64_t i) {return rand() % 64;}));
   dbi::ExternalSort sorty;
   sorty.complexSort(std::string("bin/data"), 8*64, 64);

   uint64_t last = 0;
   bool check = true;
   uint64_t i = 0;
   EXPECT_TRUE(dbiu::foreachInFile("bin/data", [&](uint64_t data) {check&=last<=data; last=data; i++;}));
   EXPECT_TRUE(check);
   EXPECT_EQ(i , 1<<10);
   remove("bin/data");
}

TEST(ExternalSort, ComplexBig_8_Pages) {
   EXPECT_TRUE(dbiu::createTestFile("bin/data", 1<<20, [&](uint64_t i) {return rand();}));
   dbi::ExternalSort sorty;
   sorty.complexSort(std::string("bin/data"), 8*1024, 1024);

   uint64_t last = 0;
   bool check = true;
   uint64_t i = 0;
   EXPECT_TRUE(dbiu::foreachInFile("bin/data", [&](uint64_t data) {check&=last<=data; last=data; i++;}));
   EXPECT_TRUE(check);
   EXPECT_EQ(i , 1<<20);
   remove("bin/data");
}

TEST(ExternalSort, ComplexBig_32_Pages) {
   EXPECT_TRUE(dbiu::createTestFile("bin/data", 1<<20, [&](uint64_t i) {return rand();}));
   dbi::ExternalSort sorty;
   sorty.complexSort(std::string("bin/data"), 32*1024, 1024);

   uint64_t last = 0;
   bool check = true;
   uint64_t i = 0;
   EXPECT_TRUE(dbiu::foreachInFile("bin/data", [&](uint64_t data) {check&=last<=data; last=data; i++;}));
   EXPECT_TRUE(check);
   EXPECT_EQ(i , 1<<20);
   remove("bin/data");
}

TEST(ExternalSort, ComplexBig_128_Pages) {
   EXPECT_TRUE(dbiu::createTestFile("bin/data", 1<<20, [&](uint64_t i) {return rand();}));
   dbi::ExternalSort sorty;
   sorty.complexSort(std::string("bin/data"), 128*1024, 1024);

   uint64_t last = 0;
   bool check = true;
   uint64_t i = 0;
   EXPECT_TRUE(dbiu::foreachInFile("bin/data", [&](uint64_t data) {check&=last<=data; last=data; i++;}));
   EXPECT_TRUE(check);
   EXPECT_EQ(i , 1<<20);
   remove("bin/data");
}
