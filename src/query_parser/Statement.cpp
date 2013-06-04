#include "Statement.hpp"
#include "Visitor.hpp"
#include "harriet/Expression.hpp"

using namespace std;

namespace dbi {

namespace script {

Statement::~Statement()
{
}

SelectStatement::SelectStatement(vector<ColumnIdentifier>&& selectors, vector<TableAccess>&& sources)
: selectors(move(selectors))
, sources(move(sources))
{
}

void SelectStatement::acceptVisitor(Visitor& visitor)
{
   visitor.onPreVisit(*this);
   visitor.onPostVisit(*this);
}

CreateTableStatement::CreateTableStatement(const string& name, vector<AttributeDeclaration>&& attributes)
: name(name)
, attributes(move(attributes))
{
}

void CreateTableStatement::acceptVisitor(Visitor& visitor)
{
   visitor.onPreVisit(*this);
   visitor.onPostVisit(*this);
}

InsertStatement::InsertStatement(const string& tableName, vector<unique_ptr<harriet::Value>>&& values)
: tableName(tableName)
, values(move(values))
{
}

void InsertStatement::acceptVisitor(Visitor& visitor)
{
   visitor.onPreVisit(*this);
   visitor.onPostVisit(*this);
}

BlockStatement::BlockStatement(vector<unique_ptr<Statement>> statements)
: statements(move(statements))
{
}

void BlockStatement::acceptVisitor(Visitor& visitor)
{
   visitor.onPreVisit(*this);
   for(auto& iter : statements)
      iter->acceptVisitor(visitor);
   visitor.onPostVisit(*this);
}

RootStatement::RootStatement(unique_ptr<BlockStatement> statement)
: statement(move(statement))
{
}

void RootStatement::acceptVisitor(Visitor& visitor)
{
   visitor.onPreVisit(*this);
   statement->acceptVisitor(visitor);
   visitor.onPostVisit(*this);
}

}

}
