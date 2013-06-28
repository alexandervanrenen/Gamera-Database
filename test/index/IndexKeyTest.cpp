#include "gtest/gtest.h"
#include "test/TestConfig.hpp"
#include "btree/IndexKey.hpp"
#include "btree/IndexKeySchema.hpp"
#include "btree/IndexKeyComparator.hpp"
#include "harriet/Value.hpp"
#include <string>

using namespace std;
using namespace harriet;
using namespace dbi;

TEST(IndexInfra, KeyTest)
{
   // First key
   vector<harriet::Value> data_2_klee;
   data_2_klee.emplace_back(harriet::Value::createInteger(2));
   data_2_klee.emplace_back(harriet::Value::createCharacter(string("klee"), 20));
   IndexKey key_2_klee(move(data_2_klee));
   vector<char> mem_2_klee(key_2_klee.bytes());
   key_2_klee.writeToMem(mem_2_klee.data());

   // Second key
   vector<harriet::Value> data_2_bee;
   data_2_bee.emplace_back(harriet::Value::createInteger(2));
   data_2_bee.emplace_back(harriet::Value::createCharacter(string("bee"), 20));
   IndexKey key_2_bee(move(data_2_bee));
   vector<char> mem_2_bee(key_2_bee.bytes());
   key_2_bee.writeToMem(mem_2_bee.data());

   // Comparator
   vector<harriet::VariableType> types = {harriet::VariableType::createIntegerType(), harriet::VariableType::createCharacterType(20)};
   IndexKeySchema schema(types);
   IndexKeyComparator comparator(schema);

   ASSERT_EQ(schema.bytes(), key_2_klee.bytes());
   ASSERT_EQ(schema.bytes(), key_2_bee.bytes());

   ASSERT_TRUE(comparator.equal(key_2_bee, key_2_bee));
   ASSERT_TRUE(comparator.equal(key_2_bee, mem_2_bee.data()));
   ASSERT_TRUE(comparator.equal(mem_2_bee.data(), key_2_bee));
   ASSERT_TRUE(comparator.equal(mem_2_bee.data(), mem_2_bee.data()));

   ASSERT_FALSE(comparator.equal(key_2_klee, key_2_bee));
   ASSERT_FALSE(comparator.equal(key_2_klee, mem_2_bee.data()));
   ASSERT_FALSE(comparator.equal(mem_2_klee.data(), key_2_bee));
   ASSERT_FALSE(comparator.equal(mem_2_klee.data(), mem_2_bee.data()));

   ASSERT_TRUE(comparator.less(key_2_bee, key_2_klee));
   ASSERT_TRUE(comparator.less(key_2_bee, mem_2_klee.data()));
   ASSERT_TRUE(comparator.less(mem_2_bee.data(), key_2_klee));
   ASSERT_TRUE(comparator.less(mem_2_bee.data(), mem_2_klee.data()));

   ASSERT_FALSE(comparator.less(key_2_klee, key_2_bee));
   ASSERT_FALSE(comparator.less(key_2_klee, mem_2_bee.data()));
   ASSERT_FALSE(comparator.less(mem_2_klee.data(), key_2_bee));
   ASSERT_FALSE(comparator.less(mem_2_klee.data(), mem_2_bee.data()));
}
