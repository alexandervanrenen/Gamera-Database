#include "PlanGenerationVisitor.hpp"
#include "Statement.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Environment.hpp"
#include "schema/RelationSchema.hpp"
#include "schema/SchemaManager.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "util/Utility.hpp"
#include "operator/SingleRecordOperator.hpp"
#include "operator/InsertOperator.hpp"
#include "operator/PrintOperator.hpp"
#include "operator/TableScanOperator.hpp"
#include "operator/ProjectionOperator.hpp"
#include "operator/SelectionOperator.hpp"
#include "operator/CrossProductOperator.hpp"
#include "query_optimizer/ChainOptimizer.hpp"
#include "query_optimizer/DummyOptimizer.hpp"
#include "query_util/TableAccessInfo.hpp"
#include "query_util/PredicateGenerator.hpp"
#include "query_util/Predicate.hpp"
#include "query_util/ColumnResolver.hpp"
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

   plan = util::make_unique<ProjectionOperator>(move(plan), projectionTargets, select.globalRegister);
   select.queryPlan = util::make_unique<PrintOperator>(move(plan), cout, select.globalRegister);
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
