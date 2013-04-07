#include "util/Utility.hpp"
#include "external_sort/ExternalSort.hpp"
#include <cstdio>
#include <iostream>

void runComplexSort(uint64_t entriesCount, uint64_t pageSize, uint64_t maxMemory, bool showPerformance = false)
{
   EXPECT_TRUE(dbiu::createTestFile("bin/datain", entriesCount, [&](uint64_t) {return rand();}));
   dbi::ExternalSort sorty("bin/datain", "bin/dataout", pageSize, maxMemory, showPerformance);
   sorty.run();

   uint64_t last = 0;
   bool check = true;
   uint64_t i = 0;
   EXPECT_TRUE(dbiu::foreachInFile("bin/dataout", [&](uint64_t data) {check&=last<=data; last=data; i++;}));
   EXPECT_TRUE(check);
   EXPECT_EQ(i , entriesCount);
   remove("bin/datain");
   remove("bin/dataout");
}

TEST(ExternalSort, ComplexSmall) {
   runComplexSort(1<<10, 64, 8*64);
}

TEST(ExternalSort, Complex_plus_one) {
   runComplexSort((1<<10) + 1, 64, 8*64);
}

TEST(ExternalSort, Complex_minus_one) {
   runComplexSort((1<<10) - 1, 64, 8*64);
}

TEST(ExternalSort, Complex_empty) {
   runComplexSort(0, 64, 8*64);
}

TEST(ExternalSort, Complex_radom) {
   for(uint32_t i=0; i<100; i++) {
      uint64_t entriesCount = rand() % (1<<8);
      uint64_t pageSize = 8 * (rand() % 16 + 1);
      uint64_t maxMemory = 3 * pageSize * (rand() % 16 + 1);
      runComplexSort(entriesCount, pageSize, maxMemory);
   }
}

const uint32_t size = 16;
const bool showPerformance = false;

TEST(ExternalSort, ComplexBig_8_Pages) {
   runComplexSort(1<<size, 1024, 8*1024, showPerformance);
}

TEST(ExternalSort, ComplexBig_32_Pages) {
   runComplexSort(1<<size, 1024, 32*1024, showPerformance);
}

TEST(ExternalSort, ComplexBig_128_Pages) {
   runComplexSort(1<<size, 1024, 128*1024, showPerformance);
}

TEST(ExternalSort, ComplexBig_512_Pages) {
   runComplexSort(1<<size, 1024, 512*1024, showPerformance);
}

TEST(ExternalSort, ComplexBig_1024_Pages) {
   runComplexSort(1<<size, 1024, 1024*1024, showPerformance);
}

TEST(ExternalSort, ComplexBig_large) {
   runComplexSort(1<<20, 256, 1024, showPerformance);
}
