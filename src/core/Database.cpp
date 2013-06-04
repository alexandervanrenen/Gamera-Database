#include "Database.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "util/Utility.hpp"
#include "DatabaseConfig.hpp"
#include "query_parser/Parser.hpp"
#include "query_parser/Statement.hpp"
#include "query_parser/Visitor.hpp"
#include "query_parser/PrintVisitor.hpp"
#include "query_parser/CodeGenerationVisitor.hpp"
#include "schema/SchemaManager.hpp"
#include "util/DynamicLinker.hpp"
#include "TransactionCallbackHandler.hpp"
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
   cout << "bye, have a good one ;)" << endl;
}

template <typename Signature>
std::function<Signature> cast(void* f)
{
    return reinterpret_cast<Signature*>(f);
}

Result Database::executeQuery(const std::string& query)
{
   string fileName = "bin/gen/test";
   try {
      // Parse query
      unique_ptr<script::Statement> root = script::parse(query);

      // Generate code
      ofstream out((fileName + ".cpp").c_str());
      script::PrintVisitor printy(cout);
      root->acceptVisitor(printy);
      return Result();

      // Compile code
      util::DynamicLinker linker;
      linker.compile(fileName, "-g3 -O0 -Isrc/");
      auto function = linker.getFunction<void(TransactionCallbackHandler&)>("entry");

      // Run the code
      TransactionCallbackHandler handler(*segmentManager, *schemaManager);
      function(handler);

   } catch(script::ParserException e) {
      cout << "unable to parse query (line: " << e.line << "; column: " << e.column << ")" << endl;
      return Result();
   }

   return Result();
}

}
