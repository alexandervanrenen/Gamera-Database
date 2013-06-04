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
   ASSERT_EQ(lhs.name, rhs.name);
   ASSERT_EQ(lhs.sid, rhs.sid);

   // Check attributes
   ASSERT_EQ(lhs.attributes.size(), rhs.attributes.size());
   for(uint32_t i=0; i<lhs.attributes.size(); i++) {
      ASSERT_EQ(lhs.attributes[i].name, rhs.attributes[i].name);
      ASSERT_EQ(lhs.attributes[i].type, rhs.attributes[i].type);
      ASSERT_EQ(lhs.attributes[i].notNull, rhs.attributes[i].notNull);
      ASSERT_EQ(lhs.attributes[i].primaryKey, rhs.attributes[i].primaryKey);
   }

   // Check indexes
   ASSERT_EQ(lhs.indexes.size(), rhs.indexes.size());
   for(uint32_t i=0; i<lhs.indexes.size(); i++) {
      ASSERT_EQ(lhs.indexes[i].sid, rhs.indexes[i].sid);
      ASSERT_EQ(lhs.indexes[i].indexedAttribute, rhs.indexes[i].indexedAttribute);
      ASSERT_EQ(lhs.indexes[i].indexType, rhs.indexes[i].indexType);
   }
}

TEST(Schema, RelationSchemaMarschalling)
{
   const uint32_t kPages = 100;
   assert(kSwapFilePages >= kPages);

   // Create
   RelationSchema original;
   original.name = "students";
   original.sid = SegmentId(8128);
   original.attributes.push_back(dbi::AttributeSchema{"id", harriet::VariableType::TInteger, false, false});
   original.attributes.push_back(dbi::AttributeSchema{"name", harriet::VariableType::TFloat, true, true});
   original.attributes.push_back(dbi::AttributeSchema{"term", harriet::VariableType::TBool, false, true});
   original.attributes.push_back(dbi::AttributeSchema{"dog", harriet::VariableType::TInteger, false, true});

   // Serialize and de-serialize
   Record r = original.marschall();
   RelationSchema copy;
   copy.unmarschall(r);

   // Compare
   compare(original, copy);
}

TEST(Schema, SchemaManager)
{
   const uint32_t pages = 100;
   assert(kSwapFilePages>=pages);

   dbi::BufferManager bufferManager(kSwapFileName, pages / 2);
   dbi::SegmentManager segmentManager(bufferManager, true);

   RelationSchema schema1;
   schema1.name = "students";
   schema1.sid = SegmentId(8128);
   schema1.attributes.push_back(dbi::AttributeSchema{"id", harriet::VariableType::TInteger, false, false});

   RelationSchema schema2;
   schema2.name = "listens_to";
   schema2.sid = SegmentId(1729);
   schema2.attributes.push_back(dbi::AttributeSchema{"name", harriet::VariableType::TBool, true, true});

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
