#include "ExecutionVisitor.hpp"
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
#include "operator/RecordScanOperator.hpp"
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
   RelationSchema schema = schemaManager.getRelation(select.sources[0].tableIdentifier);
   // schema.setAlias(select.sources.alias!=""?select.sources.alias:select.sources.tableIdentifier);
   auto& segment = segmentManager.getSPSegment(schema.getSegmentId());
   auto recordScan = util::make_unique<RecordScanOperator>(segment);
   auto tableScan = util::make_unique<TableScanOperator>(move(recordScan), schema);
   auto print = util::make_unique<PrintOperator>(move(tableScan), cout);

   print->checkTypes();
   print->execute();
}

void ExecutionVisitor::onPostVisit(SelectStatement&)
{
}

void ExecutionVisitor::onPreVisit(CreateTableStatement& createTable)
{
   // Create attributes
   vector<AttributeSchema> attributes;
   for(auto& iter : createTable.attributes)
      attributes.push_back(dbi::AttributeSchema{iter.name, harriet::nameToType(iter.type), iter.notNull, true, 0});

   // Create indexes
   vector<IndexSchema> indexes;

   // Add relation
   dbi::RelationSchema schema(createTable.name, move(attributes), move(indexes));
   SegmentId sid = segmentManager.createSegment(SegmentType::SP, kInitialPagesPerRelation);
   schema.setSegmentId(sid);
   schema.optimizePadding();
   schemaManager.addRelation(schema);
}

void ExecutionVisitor::onPostVisit(CreateTableStatement&)
{
}

void ExecutionVisitor::onPreVisit(InsertStatement& insert)
{
   auto source = util::make_unique<SingleRecordOperator>(insert.values, RelationSchema(insert.values));

   auto& targetSchema = schemaManager.getRelation(insert.tableName);
   SPSegment& targetSegment = segmentManager.getSPSegment(targetSchema.getSegmentId());
   auto plan = util::make_unique<InsertOperator>(move(source), targetSegment, targetSchema);

   plan->checkTypes();
   plan->execute();
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
