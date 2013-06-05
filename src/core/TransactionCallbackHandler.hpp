#pragma once

#include <string>
#include <vector>
#include <memory>

namespace harriet {
   class Value;
}

namespace dbi {

class RelationSchema;
class SchemaManager;
class SegmentManager;

class TransactionCallbackHandler {
public:
   TransactionCallbackHandler(SegmentManager& segmentManager, SchemaManager& schemaManager);

   /// These functions are getting called by the generated code
   /// They perform the essential high level database operations
   void createTable(RelationSchema& schema);
   void insertIntoTable(std::string& tableName, std::vector<std::unique_ptr<harriet::Value>>& values);

private:
   SegmentManager& segmentManager;
   SchemaManager& schemaManager;
};

}
