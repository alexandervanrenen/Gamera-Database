#pragma once

#include <unordered_map>
#include <memory>
#include <string>

namespace dbi {

class SPSegment;
class RelationSchema;

class SchemaManager { 
public:
    /// Constructor
    SchemaManager(SPSegment& storage);
    ~SchemaManager();

    // Manage Relations
    void addRelation(std::unique_ptr<RelationSchema> relationShema);
    void dropRelation(const std::string& relationName);

    // Get information about relations
    RelationSchema& getRelationSchema(const std::string& relationName);

private:
    /// Used to persist the schema
    SPSegment& storage;

    /// Maps the relation name to the actual schema
    std::unordered_map<std::string, std::unique_ptr<RelationSchema>> relations;
};

}
