#pragma once

#include "buffer_manager/BufferManager.hpp"
#include "common/Config.hpp"
#include "common/AttributeType.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "segment_manager/SPSegment.hpp"
#include "segment_manager/Record.hpp"
#include "sqlparser/Schema.hpp"
#include "sqlparser/Parser.hpp"
#include "Schema.hpp"
#include <vector>
#include <unordered_map> 


namespace std {
template<>
struct hash<std::pair<std::string, std::string>> {
   size_t operator()(const std::pair<std::string, std::string>& p) const {
      return hash<std::string>()(p.first) ^ hash<std::string>()(p.second);
   }
};

template<>
struct hash<std::pair<std::string, dbi::SegmentId>> {
   size_t operator()(const std::pair<std::string, dbi::SegmentId>& p) const {
      return hash<std::string>()(p.first) ^ p.second.toInteger();
   }
};
}

namespace dbi {


struct IndexMetadata {
    RelationMetadata* relation;
    SegmentId segment;
    std::vector<AttributeMetadata*> attributes;
    
    IndexMetadata() {};
    IndexMetadata(RelationMetadata* relation, SegmentId segment, std::vector<AttributeMetadata*> attributes): relation(relation), segment(segment), attributes(attributes) {}
};


class MetadataManager { 
public:
    typedef std::vector<IndexMetadata*> RelationIndexes;
    typedef sqlparser::Schema sqlSchema;
    typedef sqlparser::Schema::Relation sqlRelation;
    typedef sqlparser::Schema::Relation::Attribute sqlAttribute;
    /// Constructor
    MetadataManager(SegmentManager& sm);
    virtual ~MetadataManager() {
    }
    
    // Manage Relations
    void addRelation(sqlRelation rel);

    // Get information from relations
    SegmentId getSegmentForRelation(const std::string name);
    AttributeType getTypeForAttribute(const std::string relationName, const std::string attributeName);
    RelationIndexes getRelationIndexes(const std::string relationName);
    void addIndex(const std::string relationName, SegmentId sid, std::vector<std::string> attr);
    void setSegment(const std::string relationName, SegmentId sid);
    uint16_t getAttributeOffset(const std::string relationName, const std::string attributeName);
        
private:
    SPSegment& segRelations;
    SPSegment& segAttributes;
    
    typedef std::pair<std::string, std::string> StringPair;
    std::unordered_map<std::string, RelationMetadata*> relations; 
    std::unordered_map<TupleId, RelationMetadata*> relationsByTid; 
    std::unordered_map<StringPair, AttributeMetadata*> attributes; 
    std::unordered_map<std::string, RelationIndexes> relationIndexes;

    RelationMetadata* loadRelationMetadata(const Record& r, const TupleId& tid);
    Record* saveRelationMetadata(RelationMetadata* rm);
    void saveRelation(RelationMetadata* rm);
    AttributeMetadata* loadAttributeMetadata(const Record& r, const TupleId& tid);
    Record* saveAttributeMetadata(AttributeMetadata* am);
    void saveAttribute(AttributeMetadata* am);
    void loadData();
    void calculateRelationIndexes(RelationMetadata* rm);

    void reorderRelation(sqlRelation& r);
};


}
