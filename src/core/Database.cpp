#include "Database.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "DatabaseConfig.hpp"
#include "harriet/Environment.hpp"
#include "harriet/Expression.hpp"
#include "query/operator/TableScanOperator.hpp"
#include "query/parser/ExecutionVisitor.hpp"
#include "query/parser/Parser.hpp"
#include "query/parser/PlanGenerationVisitor.hpp"
#include "query/parser/PrintVisitor.hpp"
#include "query/parser/Statement.hpp"
#include "query/parser/Visitor.hpp"
#include "query/QueryFacade.hpp"
#include "query/result/QueryResultCollection.hpp"
#include "schema/SchemaManager.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "util/Utility.hpp"
#include <iostream>

using namespace std;

namespace dbi {

Database::Database(const DatabaseConfig& config, bool isInitialSetup)
{
   bufferManager = util::make_unique<BufferManager>(config.storageFile, config.memoryPages);
   segmentManager = util::make_unique<SegmentManager>(*bufferManager, isInitialSetup);
   schemaManager = util::make_unique<SchemaManager>(segmentManager->getSPSegment(kSchemaSegmentId));
}

Database::~Database()
{
}

unique_ptr<QueryResultCollection> Database::executeQuery(const std::string& query)
{
   harriet::Environment env;
   QueryFacade queryFacade(*segmentManager, *schemaManager);
   return queryFacade.executeQuery(query, env, true);
}

}
