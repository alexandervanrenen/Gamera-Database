#include "harriet/Environment.hpp"
#include "harriet/Expression.hpp"
#include "harriet/ScriptLanguage.hpp"
#include "query/operator/CrossProductOperator.hpp"
#include "query/operator/InsertOperator.hpp"
#include "query/operator/PrintOperator.hpp"
#include "query/operator/ProjectionOperator.hpp"
#include "query/operator/SelectionOperator.hpp"
#include "query/operator/SingleRecordOperator.hpp"
#include "query/operator/TableScanOperator.hpp"
#include "PlanGenerationVisitor.hpp"
#include "query/optimizer/ChainOptimizer.hpp"
#include "query/optimizer/DummyOptimizer.hpp"
#include "query/util/ColumnResolver.hpp"
#include "query/util/Predicate.hpp"
#include "query/util/PredicateGenerator.hpp"
#include "query/util/TableAccessInfo.hpp"
#include "schema/RelationSchema.hpp"
#include "schema/SchemaManager.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "Statement.hpp"
#include "util/Utility.hpp"
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
   for(auto& iter : select.selections) {
      requiredProjectionColums.insert(resolver.resolveProjection(iter, select.tableAccessVec));
      projectionTargets.push_back(resolver.resolveProjection(iter, select.tableAccessVec));
   }

   // Build predicates from condition-expressions
   vector<std::unique_ptr<qopt::Predicate>> predicates;
   qopt::PredicateGenerator predicateGenerator(environment);
   predicates = predicateGenerator.createPredicates(select.conditions, select.tableAccessVec);

   // Let the optimizer build a nice access tree from the table access' and the predicates
   qopt::ChainOptimizer opty(select.globalRegister);
   auto plan = opty.optimize(select.tableAccessVec, predicates, requiredProjectionColums);

   plan = util::make_unique<ProjectionOperator>(move(plan), projectionTargets);
   select.queryPlan = util::make_unique<PrintOperator>(move(plan), select.globalRegister);
}

void PlanGenerationVisitor::onPreVisit(InsertStatement& insert)
{
   auto source = util::make_unique<SingleRecordOperator>(move(insert.values), insert.globalRegister);

   auto& targetSchema = schemaManager.getRelation(insert.tableName);
   SPSegment& targetSegment = segmentManager.getSPSegment(targetSchema.getSegmentId());
   insert.queryPlan = util::make_unique<InsertOperator>(move(source), targetSegment, targetSchema, insert.globalRegister);
}

}

}
