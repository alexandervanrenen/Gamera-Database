#include "QueryFacade.hpp"
#include "harriet/Environment.hpp"
#include "harriet/ScriptLanguage.hpp"
#include "parser/Statement.hpp"
#include "query/parser/ExecutionVisitor.hpp"
#include "query/parser/Parser.hpp"
#include "query/parser/PlanGenerationVisitor.hpp"
#include "query/parser/PrintVisitor.hpp"
#include "query/result/QueryResult.hpp"
#include "query/result/QueryResultCollection.hpp"
#include "util/Utility.hpp"
#include <iostream>

using namespace std;

namespace dbi {

QueryFacade::QueryFacade(SegmentManager& segmentManager, SchemaManager& schemaManager)
: segmentManager(segmentManager)
, schemaManager(schemaManager)
{
}

QueryFacade::~QueryFacade()
{
}

unique_ptr<QueryResultCollection> QueryFacade::executeQuery(const string& query, harriet::Environment& environment, bool showPlan)
{
   auto result = util::make_unique<QueryResultCollection>();

   // Parse query
   unique_ptr<script::RootStatement> roots;
   try {
      roots = script::parse(query);
   } catch(script::ParserException& e) {
      result->setParserError(e.line, e.column);
      return result;
   }

   // Execute query
   try {
      for(auto& root : roots->statements) {
         // Plan generation
         script::PlanGenerationVisitor geny(segmentManager, schemaManager, environment);
         root->acceptVisitor(geny);

         // Print script
         ostringstream treeStream;
         if(showPlan) {
            script::PrintVisitor printy(treeStream, script::PrintVisitor::PrintMode::kSelect);
            root->acceptVisitor(printy);
            if(treeStream.str().size() != 0)
               result->addPrintOutput(treeStream.str());
         }

         // Interpret script
         script::ExecutionVisitor exy(segmentManager, schemaManager, *result);
         root->acceptVisitor(exy);
      }
   } catch(harriet::Exception& e) {
      result->setRuntimeError(e.message);
      return result;
   }

   return result;
}

}
