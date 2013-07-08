#include "harriet/VariableType.hpp"
#include "harriet/Value.hpp"
#include "external_sort/MergeSort.hpp"
#include "gtest/gtest.h"
#include "test/TestConfig.hpp"
#include "util/Utility.hpp"
#include <cstdio>
#include <iostream>

void runComplexSort(uint64_t entriesCount, uint64_t pagesize, uint64_t maxMemory)
{
    EXPECT_TRUE(dbi::util::createTestFile("datain", entriesCount, [&](uint32_t) {return rand();}));
    //return;
    std::vector<harriet::VariableType> columns;
    columns.push_back(harriet::VariableType::createIntegerType());
    dbi::IndexKeySchema schema{columns};
    ASSERT_EQ(schema.bytes(), 4);
    dbi::IndexKeyComparator c{schema};
    dbi::MergeSort sorty(pagesize, maxMemory, schema, c);
    EXPECT_EQ(sorty.externalsort("datain", "dataout"), 0);
    EXPECT_TRUE(sorty.checksort("dataout"));
    remove("bin/datain");
    remove("bin/dataout");
}

TEST(MergeSortTest, External)
{
   runComplexSort(1 << 11, 2 * 64, 10 * 64);
   runComplexSort(1 << 11, 2 * 64, 20 * 64);
}

