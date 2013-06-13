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
      ASSERT_EQ(lhs.getAttributes()[i].primaryKey, rhs.getAttributes()[i].primaryKey);
      ASSERT_EQ(lhs.getAttributes()[i].offset, rhs.getAttributes()[i].offset);
   }

   // Check indexes
   ASSERT_EQ(lhs.getIndexes().size(), rhs.getIndexes().size());
   for(uint32_t i=0; i<lhs.getIndexes().size(); i++) {
      ASSERT_EQ(lhs.getIndexes()[i].sid, rhs.getIndexes()[i].sid);
      ASSERT_EQ(lhs.getIndexes()[i].indexedAttribute, rhs.getIndexes()[i].indexedAttribute);
      ASSERT_EQ(lhs.getIndexes()[i].indexType, rhs.getIndexes()[i].indexType);
   }
}

TEST(Schema, RelationSchemaMarschalling)
{
   // Create
   vector<dbi::AttributeSchema> attributes;
   attributes.push_back(dbi::AttributeSchema{"id", harriet::VariableType::createIntegerType(), false, false, 0});
   attributes.push_back(dbi::AttributeSchema{"name", harriet::VariableType::createCharacterType(20), true, true, 0});
   attributes.push_back(dbi::AttributeSchema{"condition", harriet::VariableType::createBoolType(), false, true, 0});
   attributes.push_back(dbi::AttributeSchema{"percentage", harriet::VariableType::createFloatType(), false, true, 0});
   vector<dbi::IndexSchema> indexes;
   RelationSchema original("students", move(attributes), move(indexes));
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
   vector<dbi::AttributeSchema> attributes;
   attributes.push_back(dbi::AttributeSchema{"id", harriet::VariableType::createIntegerType(), false, false, 0});
   attributes.push_back(dbi::AttributeSchema{"name", harriet::VariableType::createCharacterType(20), true, true, 0});
   attributes.push_back(dbi::AttributeSchema{"condition", harriet::VariableType::createBoolType(), false, true, 0});
   attributes.push_back(dbi::AttributeSchema{"percentage", harriet::VariableType::createFloatType(), false, true, 0});
   vector<dbi::IndexSchema> indexes;
   RelationSchema schema("students", move(attributes), move(indexes));
   schema.setSegmentId(SegmentId(123));
   schema.optimizePadding();

   // Create tuple
   vector<unique_ptr<harriet::Value>> tuple;
   tuple.push_back(harriet::Value::createInteger(1337).evaluate());
   tuple.push_back(harriet::Value::createCharacter(string("alex"), 20).evaluate());
   tuple.push_back(harriet::Value::createBool(true).evaluate());
   tuple.push_back(harriet::Value::createFloatgit(3000.0f).evaluate());

   // Serialize and de-serialize
   Record record = schema.tupleToRecord(tuple);
   auto tupleCopy = schema.recordToTuple(record);

   // Compare
   ASSERT_EQ(tuple.size(), tupleCopy.size());
   for(uint32_t i=0; i<tuple.size(); i++)
      ASSERT_TRUE(tuple[i]->computeEq(*tupleCopy[i]).data.vbool);
}

TEST(Schema, SchemaManager)
{
   const uint32_t pages = 100;
   assert(kSwapFilePages>=pages);

   dbi::BufferManager bufferManager(kSwapFileName, pages / 2);
   dbi::SegmentManager segmentManager(bufferManager, true);

   vector<dbi::AttributeSchema> attributes1;
   attributes1.push_back(dbi::AttributeSchema{"id", harriet::VariableType::createIntegerType(), false, false, 0});
   vector<dbi::IndexSchema> indexes1;
   auto schema1 = util::make_unique<RelationSchema>("students", move(attributes1), move(indexes1));
   schema1->setSegmentId(SegmentId(8128));
   auto schema1_ref = util::make_unique<RelationSchema>(schema1->marschall()); // Just copy ..

   vector<dbi::AttributeSchema> attributes2;
   attributes2.push_back(dbi::AttributeSchema{"name", harriet::VariableType::createBoolType(), true, true, 0});
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
