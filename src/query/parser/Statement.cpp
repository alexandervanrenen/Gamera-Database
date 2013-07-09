#include "Statement.hpp"
#include "harriet/Expression.hpp"
#include "harriet/ScriptLanguage.hpp"
#include "harriet/Value.hpp"
#include "query/operator/PrintOperator.hpp"
#include "query/operator/RootOperator.hpp"
#include "query/util/GlobalRegister.hpp"
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

SelectStatement::SelectStatement(vector<pair<string, unique_ptr<harriet::Expression>>>&& selectors, vector<TableReference>&& sources, vector<unique_ptr<harriet::Expression>>&& conditions, vector<string>&& orderBy)
: projections(move(selectors))
, sources(move(sources))
, conditions(move(conditions))
, orderBy(move(orderBy))
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

CreateTableStatement::CreateTableStatement(string&& name, vector<AttributeDeclaration>&& attributes, vector<vector<string>>&& uniqueColumns)
: tableName(move(name))
, attributes(move(attributes))
, uniqueColumns(move(uniqueColumns))
{
}

void CreateTableStatement::acceptVisitor(Visitor& visitor)
{
   visitor.onPreVisit(*this);
   visitor.onPostVisit(*this);
}

InsertStatement::InsertStatement(string&& tableName, vector<harriet::Value>&& values)
: tableName(move(tableName))
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

DropTableStatement::DropTableStatement(string&& tableName)
: tableName(move(tableName))
{
}

DropTableStatement::~DropTableStatement()
{
}

void DropTableStatement::acceptVisitor(Visitor& visitor)
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
