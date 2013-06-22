#pragma once

#include "common/SegmentId.hpp"
#include "harriet/ScriptLanguage.hpp"
#include "segment_manager/Record.hpp"
#include <ios>
#include <set>
#include <string>
#include <vector>

namespace dbi {

struct ColumnSchema {
   std::string name;
   harriet::VariableType type;
   bool notNull;
   uint16_t offset;
};

struct IndexSchema {
   enum Type: uint8_t { kBTree, kHash, kBit };
   
   SegmentId sid;
   std::set<uint8_t> indexedColumns;
   Type type;
   bool unique;
};

struct RelationSchema {
   RelationSchema(); // Default constructor
   RelationSchema(const std::string& name, std::vector<ColumnSchema>&& columns, std::vector<IndexSchema>&& indexes); // Create a new schema from a create table statement
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
   const std::vector<ColumnSchema>& getAttributes() const {return attributes;}
   const std::vector<IndexSchema>& getIndexes() const {return indexes;}

   const ColumnSchema* getColumn(const std::string& name) const;

   void dump(std::ostream& os) const;

private:
   SegmentId sid;
   std::string name;
   std::vector<ColumnSchema> attributes;
   std::vector<IndexSchema> indexes;
};

}
