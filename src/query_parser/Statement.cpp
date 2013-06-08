#include "Statement.hpp"
#include "Visitor.hpp"
#include "harriet/Expression.hpp"

using namespace std;

namespace dbi {

namespace script {

Statement::~Statement()
{
}

bool Statement::isLocal() const
{
   Type t = getType();
   return t==Type::kSelectStatement || t==Type::kInsertStatement || t==Type::kBlockStatement;
}

bool Statement::isGlobal() const
{
   Type t = getType();
   return t==Type::kSelectStatement || t==Type::kCreateTableStatement || t==Type::kInsertStatement;
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
   for(auto& statement : statements)
      if(!statement->isLocal())
         throw "non local statement in local scope";
}

void BlockStatement::acceptVisitor(Visitor& visitor)
{
   visitor.onPreVisit(*this);
   for(auto& iter : statements)
      iter->acceptVisitor(visitor);
   visitor.onPostVisit(*this);
}

RootStatement::RootStatement(vector<unique_ptr<Statement>> statements)
: statements(move(statements))
{
   for(auto& statement : statements)
      if(!statement->isGlobal())
         throw "non global statement in global scope";
}

void RootStatement::acceptVisitor(Visitor& visitor)
{
   visitor.onPreVisit(*this);
   for(auto& statement : statements)
      statement->acceptVisitor(visitor);
   visitor.onPostVisit(*this);
}

}

}
