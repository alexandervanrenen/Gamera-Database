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
   RelationSchema(const RelationSchema&) = delete;
   RelationSchema& operator=(const RelationSchema&) = delete;

   std::vector<harriet::Value> recordToTuple(const Record& record) const;
   Record tupleToRecord(const std::vector<harriet::Value>& tuple) const;

   void setSegmentId(SegmentId sid);
   void optimizePadding();

   Record marschall() const;

   const SegmentId getSegmentId() const {return sid;}
   const std::string& getName() const {return name;}
   const std::vector<AttributeSchema>& getAttributes() const {return attributes;}
   const std::vector<IndexSchema>& getIndexes() const {return indexes;}

   const AttributeSchema* getAttribute(const std::string& name) const;

   void dump(std::ostream& os) const;

private:
   SegmentId sid;
   std::string name;
   std::vector<AttributeSchema> attributes;
   std::vector<IndexSchema> indexes;
};

}
