#pragma once

#include <string>

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

private:
   SegmentManager& segmentManager;
   SchemaManager& schemaManager;
};

}
