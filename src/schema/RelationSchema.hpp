#pragma once

#include "common/SegmentId.hpp"
#include <vector>
#include <string>

namespace dbi {

struct AttributeSchema {
   std::string name;
   uint8_t type;
   uint8_t len;
   bool notNull;
   bool primaryKey;
};

struct IndexSchema {
   SegmentId sid;
   uint8_t indexedAttribute;
   uint8_t indexType;
};

struct RelationSchema {
   SegmentId sid;
   std::string name;
   std::vector<AttributeSchema> attributes;
   std::vector<IndexSchema> indexes;
};

}
