#pragma once

#include "common/SegmentId.hpp"
#include "segment_manager/Record.hpp"
#include "harriet/ScriptLanguage.hpp"
#include <vector>
#include <string>
#include <ios>

namespace dbi {

struct AttributeSchema {
   std::string name;
   harriet::VariableType type;
   bool notNull;
   bool primaryKey;
   uint16_t offset;
};

struct IndexSchema {
   SegmentId sid;
   uint8_t indexedAttribute;
   uint8_t indexType;
};

struct RelationSchema {
   RelationSchema(); // Default constructor
   RelationSchema(const std::string& name, std::vector<AttributeSchema>&& attributes, std::vector<IndexSchema>&& indexes); // Create a new schema from a create table statement
   RelationSchema(const Record& record); // Load schema from raw record
   RelationSchema(const std::vector<std::unique_ptr<harriet::Value>>& values); // Deduce schema from an expression in a script

   std::vector<std::unique_ptr<harriet::Value>> getTuplefromRecord(const Record& record);
   Record getRecordFromTuple(const std::vector<std::unique_ptr<harriet::Value>>& tuple);

   void setSegmentId(SegmentId sid);
   void optimizePadding();

   Record marschall() const;
   void unmarschall();

   const SegmentId getSegmentId() const {return sid;}
   const std::string& getName() const {return name;}
   const std::vector<AttributeSchema>& getAttributes() const {return attributes;}
   const std::vector<IndexSchema>& getIndexes() const {return indexes;}

   void dump(std::ostream& os) const;

private:
   SegmentId sid;
   std::string name;
   std::vector<AttributeSchema> attributes;
   std::vector<IndexSchema> indexes;
};

}
