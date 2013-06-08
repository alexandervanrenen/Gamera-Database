#include "PlanGenerationVisitor.hpp"
#include "schema/SchemaManager.hpp"
#include "operator/SingleRecordOperator.hpp"
#include "operator/InsertOperator.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "Statement.hpp"
#include "util/Utility.hpp"

namespace dbi {

namespace script {

PlanGenerationVisitor::PlanGenerationVisitor(SchemaManager& schemaManager, SegmentManager& segmentManager, bool verbose)
: schemaManager(schemaManager)
, segmentManager(segmentManager)
, verbose(verbose)
{
}

PlanGenerationVisitor::~PlanGenerationVisitor()
{
}

void PlanGenerationVisitor::onPreVisit(InsertStatement& insert)
{
   auto source = util::make_unique<SingleRecordOperator>(insert.values, RelationSchema(insert.values));
   RelationSchema targetSchema = schemaManager.getRelation(insert.tableName);
   SPSegment& targetSegment = segmentManager.getSPSegment(targetSchema.getSegmentId());
   insert.plan = util::make_unique<InsertOperator>(move(source), targetSegment);
}

void PlanGenerationVisitor::onPostVisit(InsertStatement&)
{
   
}

}

}
