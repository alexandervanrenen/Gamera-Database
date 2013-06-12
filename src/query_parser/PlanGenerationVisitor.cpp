#include "PlanGenerationVisitor.hpp"
#include "Statement.hpp"
#include "harriet/Expression.hpp"
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
   // Cross-Product it with all other input relations
   unique_ptr<Operator> tableAccess;
   for(uint32_t i=0; i<select.sources.size(); i++) {
      const RelationSchema& sourceSchema = schemaManager.getRelation(select.sources[i].tableName);
      string tableQualifier = select.sources[i].tableQualifier!=""?select.sources[i].tableQualifier:select.sources[i].tableName;
      auto& segment = segmentManager.getSPSegment(sourceSchema.getSegmentId());
      auto nextLevel = util::make_unique<TableScanOperator>(segment, sourceSchema, tableQualifier);
      if(i==0)
         tableAccess = move(nextLevel); else
         tableAccess = util::make_unique<CrossProductOperator>(move(nextLevel), move(tableAccess));
   }

   // Create selections
   unique_ptr<Operator> last = move(tableAccess);
   for(auto& predicate : select.predicates)
      last = util::make_unique<SelectionOperator>(move(last), move(predicate));

   // Create projections
   auto projection = util::make_unique<ProjectionOperator>(move(last), select.selections);
   select.queryPlan = util::make_unique<PrintOperator>(move(projection), cout);
}

void PlanGenerationVisitor::onPreVisit(InsertStatement& insert)
{
   auto source = util::make_unique<SingleRecordOperator>(insert.values);

   auto& targetSchema = schemaManager.getRelation(insert.tableName);
   SPSegment& targetSegment = segmentManager.getSPSegment(targetSchema.getSegmentId());
   insert.queryPlan = util::make_unique<InsertOperator>(move(source), targetSegment, targetSchema);
}

}

}
