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
      ASSERT_EQ(lhs.attributes[i].len, rhs.attributes[i].len);
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
   original.sid = SegmentId(8129);
   original.attributes.push_back(dbi::AttributeSchema{"id", 2, 8, false, false});
   original.attributes.push_back(dbi::AttributeSchema{"name", 2, 2, true, true});
   original.attributes.push_back(dbi::AttributeSchema{"term", 3, 2, false, true});
   original.attributes.push_back(dbi::AttributeSchema{"term", 3, 2, false, true});

   // Serialize and de-serialize
   Record r = original.marschall();
   RelationSchema copy;
   copy.unmarschall(r);

   // Compare
   compare(original, copy);
}

TEST(Schema, SchemaManager)
{

}
