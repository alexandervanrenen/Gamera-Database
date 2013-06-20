#include "ExecutionVisitor.hpp"
#include "harriet/Expression.hpp"
#include "query/operator/RootOperator.hpp"
#include "schema/RelationSchema.hpp"
#include "schema/SchemaManager.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "Statement.hpp"
#include "util/Utility.hpp"
#include <sstream>

using namespace std;

namespace dbi {

namespace script {

ExecutionVisitor::ExecutionVisitor(SegmentManager& segmentManager, SchemaManager& schemaManager, bool verbose)
: segmentManager(segmentManager)
, schemaManager(schemaManager)
, verbose(verbose)
{
}

ExecutionVisitor::~ExecutionVisitor()
{
}

void ExecutionVisitor::onPreVisit(RootStatement&)
{
   cout << "begin query" << endl;
}

void ExecutionVisitor::onPostVisit(RootStatement&)
{
   cout << "end query" << endl;
}

void ExecutionVisitor::onPreVisit(SelectStatement& select)
{
   select.queryPlan->checkTypes();
   select.queryPlan->execute();
}

void ExecutionVisitor::onPostVisit(SelectStatement&)
{
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
}

void ExecutionVisitor::onPostVisit(CreateTableStatement&)
{
}

void ExecutionVisitor::onPreVisit(InsertStatement& insert)
{
   insert.queryPlan->checkTypes();
   insert.queryPlan->execute();
}

void ExecutionVisitor::onPostVisit(InsertStatement&)
{
}

void ExecutionVisitor::onPreVisit(BlockStatement&)
{
}

void ExecutionVisitor::onPostVisit(BlockStatement&)
{
}

}

}
