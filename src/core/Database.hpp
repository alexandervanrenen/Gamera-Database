#pragma once

#include <memory>
#include <string>

namespace dbi {

class SegmentManager;
class BufferManager;
class SchemaManager;
class DatabaseConfig;
class QueryResultCollection;

class Database {
public:
   Database(const DatabaseConfig& config, bool isInitialSetup);
   ~Database();

   std::unique_ptr<QueryResultCollection> executeQuery(const std::string& query);

private:
   std::unique_ptr<SegmentManager> segmentManager;
   std::unique_ptr<BufferManager> bufferManager;
   std::unique_ptr<SchemaManager> schemaManager;
};

}
