#include "PrintVisitor.hpp"
#include "Statement.hpp"
#include <sstream>

using namespace std;

namespace dbi {

namespace script {

PrintVisitor::PrintVisitor(std::ostream& out)
: out(out)
{
}

PrintVisitor::~PrintVisitor()
{
}

void PrintVisitor::onPreVisit(RootStatement&)
{
   out << "<<<<<<<<<<<<< root" << endl;
}

void PrintVisitor::onPostVisit(RootStatement&)
{
   out << ">>>>>>>>>>>>> root" << endl;
}

void PrintVisitor::onPreVisit(SelectStatement& select)
{
   out << "select " << endl;
   for(auto& iter : select.selectors)
      out << "    " << iter.tableIdentifier << " " << iter.columnIdentifier << endl;
   out << "from " << endl;
   for(auto& iter : select.sources)
      out << "    " << iter.tableIdentifier << " " << iter.alias << endl;
   out << ");" << endl;
}

void PrintVisitor::onPostVisit(SelectStatement&)
{
}

void PrintVisitor::onPreVisit(CreateTableStatement& createTable)
{
   out << "create table " << createTable.name << " (" << endl;
   for(auto& iter : createTable.attributes)
      out << iter.name << " " << iter.type << " " << (iter.notNull?"not null":"null") << endl;
   out << ");" << endl;
}

void PrintVisitor::onPostVisit(CreateTableStatement&)
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
