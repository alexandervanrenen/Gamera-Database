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
#include "query_optimizer/TableAccessInfo.hpp"
#include "query_optimizer/PredicateGenerator.hpp"
#include "query_optimizer/Predicate.hpp"
#include <sstream>

using namespace std;

namespace dbi {

namespace script {

PlanGenerationVisitor::PlanGenerationVisitor(SegmentManager& segmentManager, SchemaManager& schemaManager)
: segmentManager(segmentManager)
, schemaManager(schemaManager)
{
}

PlanGenerationVisitor::~PlanGenerationVisitor()
{
}

void PlanGenerationVisitor::onPreVisit(SelectStatement& select)
{
   // Build a vector containing all table access'
   vector<qopt::TableAccessInfo> tableAccessVec;
   for(uint32_t i=0; i<select.sources.size(); i++) {
      const RelationSchema* schema = &schemaManager.getRelation(select.sources[i].tableName);
      string qualifier = select.sources[i].tableQualifier!=""?select.sources[i].tableQualifier:select.sources[i].tableName;
      SPSegment* segment = &segmentManager.getSPSegment(schema->getSegmentId());
      tableAccessVec.push_back(qopt::TableAccessInfo{schema, segment, qualifier});
   }

   // Build predicates from condition-expressions
   vector<std::unique_ptr<qopt::Predicate>> predicates;
   harriet::Environment env;
   qopt::PredicateGenerator predicateGenerator(env);
   for(auto& condition : select.conditions)
      predicates.emplace_back(predicateGenerator.createPredicate(move(condition), tableAccessVec));

   // Let the optimizer build a nice access tree from the table access' and the predicates
   qopt::ChainOptimizer opty;
   auto plan = opty.optimize(tableAccessVec, predicates);

   // Create projections
   plan = util::make_unique<ProjectionOperator>(move(plan), select.selections);
   select.queryPlan = util::make_unique<PrintOperator>(move(plan), cout);
}

void PlanGenerationVisitor::onPreVisit(InsertStatement& insert)
{
   auto source = util::make_unique<SingleRecordOperator>(move(insert.values));

   auto& targetSchema = schemaManager.getRelation(insert.tableName);
   SPSegment& targetSegment = segmentManager.getSPSegment(targetSchema.getSegmentId());
   insert.queryPlan = util::make_unique<InsertOperator>(move(source), targetSegment, targetSchema);
}

}

}
