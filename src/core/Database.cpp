#include "buffer_manager/BufferManager.hpp"
#include "Database.hpp"
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
#include "schema/SchemaManager.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "util/Utility.hpp"
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
         // Plan generation
         harriet::Environment env;
         script::PlanGenerationVisitor geny(*segmentManager, *schemaManager, env);
         root->acceptVisitor(geny);

//         // Print script
//         script::PrintVisitor printy(cout, script::PrintVisitor::PrintMode::kSelect);
//         root->acceptVisitor(printy);

         // Interpret script
         script::ExecutionVisitor inty(*segmentManager, *schemaManager);
         root->acceptVisitor(inty);
      }
   } catch(script::ParserException& e) {
      cout << "unable to parse query (line: " << e.line << "; column: " << e.column << ")" << endl;
      return Result();
   }
   return Result();
}

}
