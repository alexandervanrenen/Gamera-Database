#include "PlanGenerationVisitor.hpp"
#include "harriet/Environment.hpp"
#include "harriet/Expression.hpp"
#include "harriet/ScriptLanguage.hpp"
#include "query/operator/PrintOperator.hpp"
#include "query/operator/ProjectionOperator.hpp"
#include "query/optimizer/ChainOptimizer.hpp"
#include "query/util/ColumnResolver.hpp"
#include "query/util/Predicate.hpp"
#include "query/util/PredicateGenerator.hpp"
#include "query/util/TableAccessInfo.hpp"
#include "schema/SchemaManager.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "Statement.hpp"
#include "util/Utility.hpp"
#include "query/util/GlobalRegister.hpp"
#include "segment_manager/SPSegment.hpp"
#include <sstream>

using namespace std;

namespace dbi {

namespace script {

PlanGenerationVisitor::PlanGenerationVisitor(SegmentManager& segmentManager, SchemaManager& schemaManager, const harriet::Environment& environment)
: segmentManager(segmentManager)
, schemaManager(schemaManager)
, environment(environment)
{
}

PlanGenerationVisitor::~PlanGenerationVisitor()
{
}

void PlanGenerationVisitor::onPreVisit(SelectStatement& select)
{
   // Build a vector containing all table access'
   for(uint32_t i=0; i<select.sources.size(); i++) {
      auto& relationSchema = schemaManager.getRelation(select.sources[i].tableName);
      string qualifier = select.sources[i].tableQualifier!=""?select.sources[i].tableQualifier:select.sources[i].tableName;
      auto& segment = segmentManager.getSPSegment(relationSchema.getSegmentId());
      select.tableAccessVec.push_back(qopt::TableAccessInfo{relationSchema, segment, qualifier, i});
   }

   // Build a columns needed for the projections
   qopt::ColumnResolver resolver(environment);
   set<qopt::ColumnAccessInfo> requiredProjectionColums;
   vector<qopt::ColumnAccessInfo> projectionTargets; // These tow are redundant... this will change with the Projection class
   for(auto& iter : select.projections) {
      if(resolver.resolveColumnReference(iter, select.tableAccessVec).has()) {
         requiredProjectionColums.insert(resolver.resolveColumnReference(iter, select.tableAccessVec).get());
         projectionTargets.push_back(resolver.resolveColumnReference(iter, select.tableAccessVec).get());
      } else {
         throw;
      }
   }

   // Build predicates from condition-expressions
   qopt::PredicateGenerator predicateGenerator(environment);
   vector<std::unique_ptr<qopt::Predicate>> predicates = predicateGenerator.createPredicates(select.conditions, select.tableAccessVec);
   select.globalRegister = util::make_unique<qopt::GlobalRegister>(requiredProjectionColums, predicates);

   // Let the optimizer build a nice access tree from the table access' and the predicates
   qopt::ChainOptimizer opty(*select.globalRegister, environment);
   auto plan = opty.optimize(select.tableAccessVec, predicates);

   auto projection = util::make_unique<ProjectionOperator>(move(plan), projectionTargets);
   select.queryPlan = util::make_unique<PrintOperator>(move(projection), *select.globalRegister);
}

void PlanGenerationVisitor::onPreVisit(InsertStatement& insert)
{
   // Simple case: A single tuple is provided => just insert it
   {
      // Get target relation
      auto& targetSchema = schemaManager.getRelation(insert.tableName);

      // See if we can insert the provided types into the table
      if(insert.values.size() != targetSchema.getAttributes().size())
         throw harriet::Exception{"Insert " + targetSchema.getName() + ": expected " + to_string(targetSchema.getAttributes().size()) + " arguments, " + to_string(insert.values.size()) + " provided."};
      for(uint32_t i=0; i<insert.values.size(); i++)
         if(!harriet::isImplicitCastPossible(insert.values[i].type, targetSchema.getAttributes()[i].type))
            throw harriet::Exception{"Insert into " + targetSchema.getName() + ": invalid conversion from '" + insert.values[i].type.str() + "' to '" + targetSchema.getAttributes()[i].type.str() + "' for argument " + to_string(i) + "."};
   }
}

}

}
