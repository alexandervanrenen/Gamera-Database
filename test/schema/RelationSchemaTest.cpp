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

TEST(Schema, RelationSchemaMarschalling)
{
   const uint32_t kPages = 100;
   assert(kSwapFilePages >= kPages);

   // Create
   RelationSchema original;
   original.name = "students";
   original.sid = SegmentId(8129);
   original.attributes.push_back(dbi::AttributeSchema{"id", 2, 8, false, false});
   original.attributes.push_back(dbi::AttributeSchema{"name", 2, 2, true, true});
   original.attributes.push_back(dbi::AttributeSchema{"term", 3, 2, false, true});
   original.attributes.push_back(dbi::AttributeSchema{"term", 3, 2, false, true});

   // Serialize and de-serialize
   Record r = original.marschall();
   RelationSchema copy;
   copy.unmarschall(r);
   ASSERT_EQ(original.name, copy.name);
   ASSERT_EQ(original.sid, copy.sid);

   // Check attributes
   ASSERT_EQ(original.attributes.size(), copy.attributes.size());
   for(uint32_t i=0; i<original.attributes.size(); i++) {
      ASSERT_EQ(original.attributes[i].name, copy.attributes[i].name);
      ASSERT_EQ(original.attributes[i].type, copy.attributes[i].type);
      ASSERT_EQ(original.attributes[i].len, copy.attributes[i].len);
      ASSERT_EQ(original.attributes[i].notNull, copy.attributes[i].notNull);
      ASSERT_EQ(original.attributes[i].primaryKey, copy.attributes[i].primaryKey);
   }

   // Check indexes
   ASSERT_EQ(original.indexes.size(), copy.indexes.size());
   for(uint32_t i=0; i<original.indexes.size(); i++) {
      ASSERT_EQ(original.indexes[i].sid, copy.indexes[i].sid);
      ASSERT_EQ(original.indexes[i].indexedAttribute, copy.indexes[i].indexedAttribute);
      ASSERT_EQ(original.indexes[i].indexType, copy.indexes[i].indexType);
   }
}

TEST(Schema, SchemaManager)
{

}
