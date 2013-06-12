
#include "PrintVisitor.hpp"
#include "Statement.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Environment.hpp"
#include "harriet/Value.hpp"
#include <sstream>

using namespace std;

namespace dbi {

namespace script {

PrintVisitor::PrintVisitor(std::ostream& out, PrintMode printMode)
: printMode(printMode)
, out(out)
{
}

PrintVisitor::~PrintVisitor()
{
}

void PrintVisitor::onPreVisit(RootStatement&)
{
}

void PrintVisitor::onPostVisit(RootStatement&)
{
}

void PrintVisitor::onPreVisit(SelectStatement& select)
{
   if(printMode==PrintMode::kAll || printMode==PrintMode::kSelect) {
      out << "select " << endl;
      for(auto& iter : select.selections)
         out << "    " << iter.tableQualifier << "." << iter.columnName << endl;
      out << "from " << endl;
      for(auto& iter : select.sources)
         out << "    " << iter.tableName << " " << iter.tableQualifier << endl;
      out << "where " << endl;
      for(auto& iter : select.predicates) {
         out << "    ";
         iter->print(cout);
         out << endl;
      }
      out << ";" << endl;
   }
}

void PrintVisitor::onPostVisit(SelectStatement&)
{
}

void PrintVisitor::onPreVisit(CreateTableStatement& createTable)
{
   if(printMode==PrintMode::kAll || printMode==PrintMode::kCreate) {
      out << "create table " << createTable.tableName << " (" << endl;
      for(auto& iter : createTable.attributes)
         out << iter.name << " " << iter.type.str() << " " << (iter.notNull?"not null":"null") << endl;
      out << ");" << endl;
   }
}

void PrintVisitor::onPostVisit(CreateTableStatement&)
{
}

void PrintVisitor::onPreVisit(InsertStatement& insert)
{
   if(printMode==PrintMode::kAll || printMode==PrintMode::kInsert) {
      out << "insert into " << insert.tableName << " values(" << endl;
      for(auto& iter : insert.values)
         out << *iter << endl;
      out << ");" << endl;
   }
}

void PrintVisitor::onPostVisit(InsertStatement&)
{
}

void PrintVisitor::onPreVisit(BlockStatement&)
{
   out << "{" << endl;
}

void PrintVisitor::onPostVisit(BlockStatement&)
{
   out << "}" << endl;
}

}

}
