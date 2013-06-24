#include "Statement.hpp"
#include "harriet/Expression.hpp"
#include "harriet/ScriptLanguage.hpp"
#include "harriet/Value.hpp"
#include "query/operator/PrintOperator.hpp"
#include "query/operator/RootOperator.hpp"
#include "Visitor.hpp"

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

SelectStatement::SelectStatement(vector<ColumnReference>&& selectors, vector<TableReference>&& sources, vector<unique_ptr<harriet::Expression>>&& conditions)
: projections(move(selectors))
, sources(move(sources))
, conditions(move(conditions))
{
}

SelectStatement::~SelectStatement()
{
}

void SelectStatement::acceptVisitor(Visitor& visitor)
{
   visitor.onPreVisit(*this);
   visitor.onPostVisit(*this);
}

CreateTableStatement::CreateTableStatement(const string& name, vector<AttributeDeclaration>&& attributes)
: tableName(name)
, attributes(move(attributes))
{
}

void CreateTableStatement::acceptVisitor(Visitor& visitor)
{
   visitor.onPreVisit(*this);
   visitor.onPostVisit(*this);
}

InsertStatement::InsertStatement(const string& tableName, vector<harriet::Value>&& values)
: tableName(tableName)
, values(move(values))
{
}

InsertStatement::~InsertStatement()
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