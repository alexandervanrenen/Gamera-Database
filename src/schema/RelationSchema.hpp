#pragma once

#include "common/SegmentId.hpp"
#include "segment_manager/Record.hpp"
#include "harriet/ScriptLanguage.hpp"
#include <vector>
#include <string>

namespace dbi {

struct AttributeSchema {
   std::string name;
   harriet::VariableType type;
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

   const AttributeSchema& getAttribute(const std::string& variableName) const;

   Record marschall() const;
   void unmarschall(const Record& record);
};

}
