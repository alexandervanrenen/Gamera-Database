#include "util/Utility.hpp"
#include "common/Config.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "test/TestConfig.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "schema/SchemaManager.hpp"
#include "segment_manager/SPSegment.hpp"
#include "schema/RelationSchema.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Value.hpp"
#include "gtest/gtest.h"
#include <string>

using namespace std;
using namespace dbi;

void compare(const RelationSchema& lhs, const RelationSchema& rhs)
{
   ASSERT_EQ(lhs.getName(), rhs.getName());
   ASSERT_EQ(lhs.getSegmentId(), rhs.getSegmentId());

   // Check attributes
   ASSERT_EQ(lhs.getAttributes().size(), rhs.getAttributes().size());
   for(uint32_t i=0; i<lhs.getAttributes().size(); i++) {
      ASSERT_EQ(lhs.getAttributes()[i].name, rhs.getAttributes()[i].name);
      ASSERT_EQ(lhs.getAttributes()[i].type, rhs.getAttributes()[i].type);
      ASSERT_EQ(lhs.getAttributes()[i].notNull, rhs.getAttributes()[i].notNull);
      ASSERT_EQ(lhs.getAttributes()[i].offset, rhs.getAttributes()[i].offset);
   }

   // Check indexes
   ASSERT_EQ(lhs.getIndexes().size(), rhs.getIndexes().size());
   for(uint32_t i=0; i<lhs.getIndexes().size(); i++) {
      ASSERT_EQ(lhs.getIndexes()[i].sid, rhs.getIndexes()[i].sid);
      ASSERT_EQ(lhs.getIndexes()[i].indexedColumns, rhs.getIndexes()[i].indexedColumns);
      ASSERT_EQ(lhs.getIndexes()[i].type, rhs.getIndexes()[i].type);
   }
}

TEST(Schema, RelationSchemaMarschalling)
{
   // Create
   vector<dbi::ColumnSchema> columns;
   columns.push_back(dbi::ColumnSchema{"id", harriet::VariableType::createIntegerType(), false, 0});
   columns.push_back(dbi::ColumnSchema{"name", harriet::VariableType::createCharacterType(20), true, 0});
   columns.push_back(dbi::ColumnSchema{"condition", harriet::VariableType::createBoolType(), false, 0});
   columns.push_back(dbi::ColumnSchema{"percentage", harriet::VariableType::createFloatType(), false, 0});
   vector<dbi::IndexSchema> indexes;
   indexes.push_back(IndexSchema{SegmentId(10), {1,2}, dbi::IndexSchema::Type::kBTree, true});
   RelationSchema original("students", move(columns), move(indexes));
   original.setSegmentId(SegmentId(8128));
   original.optimizePadding();

   // Serialize and de-serialize
   Record r = original.marschall();
   RelationSchema copy(r);

   // Compare
   compare(original, copy);
}

TEST(Schema, TupleMarschalling)
{
   // Create schema
   vector<dbi::ColumnSchema> attributes;
   attributes.push_back(dbi::ColumnSchema{"id", harriet::VariableType::createIntegerType(), false, 0});
   attributes.push_back(dbi::ColumnSchema{"name", harriet::VariableType::createCharacterType(20), true, 0});
   attributes.push_back(dbi::ColumnSchema{"condition", harriet::VariableType::createBoolType(), false, 0});
   attributes.push_back(dbi::ColumnSchema{"percentage", harriet::VariableType::createFloatType(), false, 0});
   vector<dbi::IndexSchema> indexes;
   RelationSchema schema("students", move(attributes), move(indexes));
   schema.setSegmentId(SegmentId(123));
   schema.optimizePadding();

   // Create tuple
   vector<harriet::Value> tuple;
   tuple.push_back(harriet::Value::createInteger(1337));
   tuple.push_back(harriet::Value::createCharacter(string("alex"), 20));
   tuple.push_back(harriet::Value::createBool(true));
   tuple.push_back(harriet::Value::createFloat(3000.0f));

   // Serialize and de-serialize
   Record record = schema.tupleToRecord(tuple);
   auto tupleCopy = schema.recordToTuple(record);

   // Compare
   ASSERT_EQ(tuple.size(), tupleCopy.size());
   for(uint32_t i=0; i<tuple.size(); i++)
      ASSERT_TRUE(tuple[i].computeEq(tupleCopy[i]).data.vbool);
}

TEST(Schema, SchemaManager)
{
   const uint32_t pages = 100;
   assert(kSwapFilePages>=pages);

   dbi::BufferManager bufferManager(kSwapFileName, pages / 2);
   dbi::SegmentManager segmentManager(bufferManager, true);

   vector<dbi::ColumnSchema> attributes1;
   attributes1.push_back(dbi::ColumnSchema{"id", harriet::VariableType::createIntegerType(), false, 0});
   vector<dbi::IndexSchema> indexes1;
   auto schema1 = util::make_unique<RelationSchema>("students", move(attributes1), move(indexes1));
   schema1->setSegmentId(SegmentId(8128));
   auto schema1_ref = util::make_unique<RelationSchema>(schema1->marschall()); // Just copy ..

   vector<dbi::ColumnSchema> attributes2;
   attributes2.push_back(dbi::ColumnSchema{"name", harriet::VariableType::createBoolType(), true, 0});
   vector<dbi::IndexSchema> indexes2;
   auto schema2 = util::make_unique<RelationSchema>("listens_to", move(attributes2), move(indexes2));
   schema2->setSegmentId(SegmentId(1729));
   auto schema2_ref = util::make_unique<RelationSchema>(schema2->marschall()); // Just copy ..

   // Set up schema manager and add two relations
   {
      dbi::SchemaManager schemaManager(segmentManager.getSPSegment(kSchemaSegmentId));
      schemaManager.addRelation(move(schema1));
      schemaManager.addRelation(move(schema2));
   }

   // Restart schema manager
   {
      dbi::SchemaManager schemaManager(segmentManager.getSPSegment(kSchemaSegmentId));

      ASSERT_TRUE(schemaManager.hasRelation("students"));
      ASSERT_TRUE(schemaManager.hasRelation("listens_to"));

      compare(*schema1_ref, schemaManager.getRelation("students"));
      compare(*schema2_ref, schemaManager.getRelation("listens_to"));

      schemaManager.dropRelation("students");
      schemaManager.dropRelation("listens_to");
      ASSERT_FALSE(schemaManager.hasRelation("students"));
      ASSERT_FALSE(schemaManager.hasRelation("listens_to"));
   }

   // Restart schema manager
   {
      dbi::SchemaManager schemaManager(segmentManager.getSPSegment(kSchemaSegmentId));
      ASSERT_FALSE(schemaManager.hasRelation("students"));
      ASSERT_FALSE(schemaManager.hasRelation("listens_to"));
   }
}
