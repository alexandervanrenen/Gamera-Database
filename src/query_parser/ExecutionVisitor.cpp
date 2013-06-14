#include "ExecutionVisitor.hpp"
#include "Statement.hpp"
#include "harriet/Expression.hpp"
#include "schema/RelationSchema.hpp"
#include "schema/SchemaManager.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "util/Utility.hpp"
#include "operator/RootOperator.hpp"
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
   select.queryPlan->dump(cout);
   cout << endl;
   select.queryPlan->checkTypes();
   select.queryPlan->execute();
}

void ExecutionVisitor::onPostVisit(SelectStatement&)
{
}

void ExecutionVisitor::onPreVisit(CreateTableStatement& createTable)
{
   // Create attributes
   vector<AttributeSchema> attributes;
   for(auto& iter : createTable.attributes)
      attributes.push_back(dbi::AttributeSchema{iter.name, iter.type, iter.notNull, true, 0});

   // Create indexes
   vector<IndexSchema> indexes;

   // Add relation
   auto schema = util::make_unique<RelationSchema>(createTable.tableName, move(attributes), move(indexes));
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
