#include "Database.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "util/Utility.hpp"
#include "DatabaseConfig.hpp"
#include "query_parser/Parser.hpp"
#include "query_parser/Statement.hpp"
#include "query_parser/Visitor.hpp"
#include "query_parser/PrintVisitor.hpp"
#include "query_parser/ExecutionVisitor.hpp"
#include "schema/SchemaManager.hpp"
#include "operator/TableScanOperator.hpp"
#include "harriet/Expression.hpp"
#include <iostream>

using namespace std;

namespace dbi {

Database::Database(const DatabaseConfig& config, bool isInitialSetup)
: config(config)
{
   bufferManager = util::make_unique<BufferManager>(config.storageFile, config.memoryPages);
   segmentManager = util::make_unique<SegmentManager>(*bufferManager, isInitialSetup);
   schemaManager = util::make_unique<SchemaManager>(segmentManager->getSPSegment(kSchemaSegmentId));
}

Database::~Database()
{
   cout << "-" << endl << "bye, have a good one ;)" << endl;
}

template <typename Signature>
std::function<Signature> cast(void* f)
{
    return reinterpret_cast<Signature*>(f);
}

Result Database::executeQuery(const std::string& query)
{
   try {
      // Parse query
      auto roots = script::parse(query);
      for(auto& root : roots->statements) {
         // Print script
         script::PrintVisitor printy(cout);
         root->acceptVisitor(printy);
         cout << endl;

         // Interpret script
         script::ExecutionVisitor inty(*segmentManager, *schemaManager);
         root->acceptVisitor(inty);
      }
   } catch(script::ParserException e) {
      cout << "unable to parse query (line: " << e.line << "; column: " << e.column << ")" << endl;
      return Result();
   }
   return Result();
}

}
