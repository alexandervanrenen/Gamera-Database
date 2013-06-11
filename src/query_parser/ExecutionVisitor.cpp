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
#include "operator/ProjectionOperator.hpp"
#include "operator/SelectionOperator.hpp"
#include "operator/CrossProductOperator.hpp"
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
   auto print = util::make_unique<PrintOperator>(move(projection), cout);

   print->dump(cout);
   cout << endl;
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
      attributes.push_back(dbi::AttributeSchema{iter.name, iter.type, iter.length, iter.notNull, true, 0});

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
   auto source = util::make_unique<SingleRecordOperator>(insert.values);

   auto& targetSchema = schemaManager.getRelation(insert.tableName);
   SPSegment& targetSegment = segmentManager.getSPSegment(targetSchema.getSegmentId());
   auto plan = util::make_unique<InsertOperator>(move(source), targetSegment, targetSchema);

   plan->dump(cout);
   cout << endl;
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
