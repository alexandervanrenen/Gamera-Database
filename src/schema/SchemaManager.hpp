#pragma once

#include "common/Config.hpp"
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
    void addRelation(RelationSchema& relationShema);
    bool hasRelation(const std::string& relationName) const;
    void dropRelation(const std::string& relationName);

    // Get information about relations
    const RelationSchema& getRelation(const std::string& relationName) const;

private:
    /// Used to persist the schema
    SPSegment& storage;

    /// Maps the relation name to the actual schema
    std::unordered_map<std::string, std::pair<TupleId, std::unique_ptr<RelationSchema>>> relations;
};

}
