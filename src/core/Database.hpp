#pragma once

#include <memory>
#include <string>

namespace dbi {

class SegmentManager;
class BufferManager;
class SchemaManager;
class DatabaseConfig;

struct Result {};

class Database {
public:
   Database(const DatabaseConfig& config, bool isInitialSetup);
   ~Database();

   Result executeQuery(const std::string& query);

private:
   const DatabaseConfig& config;
   std::unique_ptr<SegmentManager> segmentManager;
   std::unique_ptr<BufferManager> bufferManager;
   std::unique_ptr<SchemaManager> schemaManager;
};

}
