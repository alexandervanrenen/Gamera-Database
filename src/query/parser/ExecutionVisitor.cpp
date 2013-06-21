#include "ExecutionVisitor.hpp"
#include "harriet/Expression.hpp"
#include "query/operator/PrintOperator.hpp"
#include "query/operator/RootOperator.hpp"
#include "query/result/QueryResultCollection.hpp"
#include "schema/RelationSchema.hpp"
#include "schema/SchemaManager.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "Statement.hpp"
#include "util/Utility.hpp"
#include <sstream>

using namespace std;

namespace dbi {

namespace script {

ExecutionVisitor::ExecutionVisitor(SegmentManager& segmentManager, SchemaManager& schemaManager, QueryResultCollection& result)
: segmentManager(segmentManager)
, schemaManager(schemaManager)
, result(result)
{
}

ExecutionVisitor::~ExecutionVisitor()
{
}

void ExecutionVisitor::onPreVisit(SelectStatement& select)
{
   select.queryPlan->checkTypes();
   select.queryPlan->execute();
   result.addSelect(select.queryPlan->getExecutionTime(), select.queryPlan->getResult(), select.queryPlan->getSuppliedColumns());
}

void ExecutionVisitor::onPreVisit(CreateTableStatement& createTable)
{
   // Create attributes
   vector<ColumnSchema> columns;
   for(auto& iter : createTable.attributes)
      columns.push_back(dbi::ColumnSchema{iter.name, iter.type, iter.notNull, 0});

   // Create indexes
   vector<IndexSchema> indexes;

   // Add relation
   auto schema = util::make_unique<RelationSchema>(createTable.tableName, move(columns), move(indexes));
   SegmentId sid = segmentManager.createSegment(SegmentType::SP, kInitialPagesPerRelation);
   schema->setSegmentId(sid);
   schema->optimizePadding();
   schemaManager.addRelation(move(schema));
   result.addCreate(chrono::nanoseconds(-1), createTable.tableName);
}

void ExecutionVisitor::onPreVisit(InsertStatement& insert)
{
   insert.queryPlan->checkTypes();
   insert.queryPlan->execute();
   result.addInsert(chrono::nanoseconds(-1), insert.tableName);
}

}

}
