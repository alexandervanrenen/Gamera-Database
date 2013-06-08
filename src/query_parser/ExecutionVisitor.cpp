#include "ExecutionVisitor.hpp"
#include "Statement.hpp"
#include "harriet/Expression.hpp"
#include "schema/RelationSchema.hpp"
#include "core/TransactionCallbackHandler.hpp"
#include <sstream>

using namespace std;

namespace dbi {

namespace script {

ExecutionVisitor::ExecutionVisitor(TransactionCallbackHandler& transaction)
: transaction(transaction)
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

void ExecutionVisitor::onPreVisit(SelectStatement&)
{
}

void ExecutionVisitor::onPostVisit(SelectStatement&)
{
}

void ExecutionVisitor::onPreVisit(CreateTableStatement& createTable)
{
   // Create attributes
   vector<AttributeSchema> attributes;
   for(auto& iter : createTable.attributes)
      attributes.push_back(dbi::AttributeSchema{iter.name, harriet::nameToType(iter.type), iter.notNull, true});

   // Create indexes
   vector<IndexSchema> indexes;

   // Add relation
   dbi::RelationSchema schema(createTable.name, move(attributes), move(indexes));
   transaction.createTable(schema);
}

void ExecutionVisitor::onPostVisit(CreateTableStatement&)
{
}

void ExecutionVisitor::onPreVisit(InsertStatement& insert)
{
   transaction.insertIntoTable(insert.tableName, insert.values);
}

void ExecutionVisitor::onPostVisit(InsertStatement& insert)
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
