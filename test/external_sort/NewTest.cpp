#include "harriet/VariableType.hpp"
#include "harriet/Value.hpp"
#include "extsort/MergeSort.hpp"
#include "gtest/gtest.h"
#include "test/TestConfig.hpp"
#include "util/Utility.hpp"
#include <cstdio>
#include <iostream>

void runComplexSort(uint64_t entriesCount, uint64_t pagesize, uint64_t maxMemory)
{
    EXPECT_TRUE(dbi::util::createTestFile("datain", entriesCount, [&](uint64_t) {return rand();}));
    std::vector<harriet::VariableType> columns;
    columns.push_back(harriet::VariableType::createIntegerType());
    dbi::IndexKeySchema schema{columns};
    ASSERT_EQ(schema.bytes(), 4);
    dbi::IndexKeyComparator c{schema};
    dbi::MergeSort sorty(pagesize, maxMemory, schema, c);
    EXPECT_EQ(sorty.externalsort("datain", "dataout"), 0);
    /*
    uint64_t last = 0;
    bool check = true;
    uint64_t i = 0;
    EXPECT_TRUE(dbi::util::foreachInFile("bin/dataout", [&](uint64_t data) {check&=last<=data; last=data; i++;}));
    EXPECT_TRUE(check);
    EXPECT_EQ(i, entriesCount);
    */
    remove("bin/datain");
    remove("bin/dataout");
}

TEST(NewTest, ComplexSmall)
{
   runComplexSort(1 << 10, 10* 64, 100 * 64);
}

