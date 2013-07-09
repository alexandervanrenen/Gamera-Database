#include "harriet/VariableType.hpp"
#include "harriet/Value.hpp"
#include "external_sort/MergeSort.hpp"
#include "external_sort/ExternalSort.hpp"
#include "gtest/gtest.h"
#include "test/TestConfig.hpp"
#include "util/Utility.hpp"
#include <cstdio>
#include <iostream>

TEST(MergeSortTest, Simple1)
{
   // Set up infrastructure
   std::vector<harriet::VariableType> columns;
   columns.push_back(harriet::VariableType::createIntegerType());
   dbi::IndexKeySchema schema{columns};
   dbi::IndexKeyComparator c{schema};
   dbi::ExternalSort sorty(schema, c);

   // Add values
   std::vector<harriet::Value> tuple1;
   tuple1.emplace_back(harriet::Value::createInteger(1));
   auto key1 = dbi::IndexKey(std::vector<harriet::Value*>{&tuple1[0]});
   sorty.addTuple(key1);
   std::vector<harriet::Value> tuple2;
   tuple2.emplace_back(harriet::Value::createInteger(2));
   auto key2 = dbi::IndexKey(std::vector<harriet::Value*>{&tuple2[0]});
   sorty.addTuple(key2);
   std::vector<harriet::Value> tuple3;
   tuple3.emplace_back(harriet::Value::createInteger(3));
   auto key3 = dbi::IndexKey(std::vector<harriet::Value*>{&tuple3[0]});
   sorty.addTuple(key3);
   std::vector<harriet::Value> tuple4;
   tuple4.emplace_back(harriet::Value::createInteger(5));
   auto key4 = dbi::IndexKey(std::vector<harriet::Value*>{&tuple4[0]});
   sorty.addTuple(key4);

   // Do sort
   sorty.sort();

   // Check result
   ASSERT_TRUE(c.equal(sorty.readNextTuple(), key1));
   ASSERT_TRUE(c.equal(sorty.readNextTuple(), key2));
   ASSERT_TRUE(c.equal(sorty.readNextTuple(), key3));
   ASSERT_TRUE(c.equal(sorty.readNextTuple(), key4));
}

TEST(MergeSortTest, Simple2)
{
   // Set up infrastructure
   std::vector<harriet::VariableType> columns;
   columns.push_back(harriet::VariableType::createIntegerType());
   columns.push_back(harriet::VariableType::createCharacterType(10));
   dbi::IndexKeySchema schema{columns};
   dbi::IndexKeyComparator c{schema};
   dbi::ExternalSort sorty(schema, c);

   // Add values
   std::vector<harriet::Value> tuple1;
   tuple1.emplace_back(harriet::Value::createInteger(1));
   tuple1.emplace_back(harriet::Value::createCharacter(string("aaa"), 10));
   auto key1 = dbi::IndexKey(std::vector<harriet::Value*>{&tuple1[0], &tuple1[1]});
   sorty.addTuple(key1);
   std::vector<harriet::Value> tuple2;
   tuple2.emplace_back(harriet::Value::createInteger(2));
   tuple2.emplace_back(harriet::Value::createCharacter(string("ccc"), 10));
   auto key2 = dbi::IndexKey(std::vector<harriet::Value*>{&tuple2[0], &tuple2[1]});
   sorty.addTuple(key2);
   std::vector<harriet::Value> tuple3;
   tuple3.emplace_back(harriet::Value::createInteger(3));
   tuple3.emplace_back(harriet::Value::createCharacter(string("sss"), 10));
   auto key3 = dbi::IndexKey(std::vector<harriet::Value*>{&tuple3[0], &tuple3[1]});
   sorty.addTuple(key3);
   std::vector<harriet::Value> tuple4;
   tuple4.emplace_back(harriet::Value::createInteger(5));
   tuple4.emplace_back(harriet::Value::createCharacter(string("bbb"), 10));
   auto key4 = dbi::IndexKey(std::vector<harriet::Value*>{&tuple4[0], &tuple4[1]});
   sorty.addTuple(key4);

   // Do sort
   sorty.sort();

   // Check result
   ASSERT_TRUE(c.equal(sorty.readNextTuple(), key1));
   ASSERT_TRUE(c.equal(sorty.readNextTuple(), key2));
   ASSERT_TRUE(c.equal(sorty.readNextTuple(), key3));
   ASSERT_TRUE(c.equal(sorty.readNextTuple(), key4));
}

void runComplexSort(uint64_t entriesCount, uint64_t pagesize, uint64_t maxMemory)
{
    EXPECT_TRUE(dbi::util::createTestFile("bin/datain", entriesCount, [&](uint32_t) {return rand();}));
    //return;
    std::vector<harriet::VariableType> columns;
    columns.push_back(harriet::VariableType::createIntegerType());
    dbi::IndexKeySchema schema{columns};
    ASSERT_EQ(schema.bytes(), 4);
    dbi::IndexKeyComparator c{schema};
    dbi::MergeSort sorty(pagesize, maxMemory, schema, c);
    EXPECT_EQ(sorty.externalsort("bin/datain", "bin/dataout"), 0);
    EXPECT_TRUE(sorty.checksort("bin/dataout"));
    remove("bin/datain");
    remove("bin/dataout");
}

TEST(MergeSortTest, External)
{
   runComplexSort(1 << 11, 2 * 64, 10 * 64);
   runComplexSort(1 << 11, 2 * 64, 20 * 64);
   runComplexSort(1, 2 * 64, 20 * 64);
   runComplexSort(3, 2 * 64, 20 * 64);
}
