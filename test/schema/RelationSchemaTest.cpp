#include "util/Utility.hpp"
#include "common/Config.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "test/TestConfig.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "schema/SchemaManager.hpp"
#include "segment_manager/SPSegment.hpp"
#include "schema/RelationSchema.hpp"
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
   const uint32_t kPages = 100;
   assert(kSwapFilePages >= kPages);

   // Create
   vector<dbi::AttributeSchema> attributes;
   attributes.push_back(dbi::AttributeSchema{"id", harriet::VariableType::TInteger, false, false});
   attributes.push_back(dbi::AttributeSchema{"name", harriet::VariableType::TFloat, true, true});
   attributes.push_back(dbi::AttributeSchema{"term", harriet::VariableType::TBool, false, true});
   attributes.push_back(dbi::AttributeSchema{"dog", harriet::VariableType::TInteger, false, true});
   vector<dbi::IndexSchema> indexes;
   RelationSchema original("students", move(attributes), move(indexes));
   original.setSegmentId(SegmentId(8128));

   // Serialize and de-serialize
   Record r = original.marschall();
   RelationSchema copy(r);

   // Compare
   compare(original, copy);
}

TEST(Schema, SchemaManager)
{
   const uint32_t pages = 100;
   assert(kSwapFilePages>=pages);

   dbi::BufferManager bufferManager(kSwapFileName, pages / 2);
   dbi::SegmentManager segmentManager(bufferManager, true);

   vector<dbi::AttributeSchema> attributes1;
   attributes1.push_back(dbi::AttributeSchema{"id", harriet::VariableType::TInteger, false, false});
   vector<dbi::IndexSchema> indexes1;
   RelationSchema schema1("students", move(attributes1), move(indexes1));
   schema1.setSegmentId(SegmentId(8128));

   vector<dbi::AttributeSchema> attributes2;
   attributes2.push_back(dbi::AttributeSchema{"name", harriet::VariableType::TBool, true, true});
   vector<dbi::IndexSchema> indexes2;
   RelationSchema schema2("listens_to", move(attributes2), move(indexes2));
   schema2.setSegmentId(SegmentId(1729));

   // Set up schema manager and add two relations
   {
      dbi::SchemaManager schemaManager(segmentManager.getSPSegment(kSchemaSegmentId));
      schemaManager.addRelation(schema1);
      schemaManager.addRelation(schema2);
   }

   // Restart schema manager
   {
      dbi::SchemaManager schemaManager(segmentManager.getSPSegment(kSchemaSegmentId));

      ASSERT_TRUE(schemaManager.hasRelation("students"));
      ASSERT_TRUE(schemaManager.hasRelation("listens_to"));
      compare(schema1, schemaManager.getRelation("students"));
      compare(schema2, schemaManager.getRelation("listens_to"));

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
