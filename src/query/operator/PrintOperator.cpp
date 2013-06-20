#include "harriet/ScriptLanguage.hpp"
#include "harriet/Value.hpp"
#include "Operator.hpp"
#include "PrintOperator.hpp"
#include "query/signature/ColumnSignature.hpp"
#include "query/signature/Signature.hpp"
#include "util/Utility.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>

using namespace std;

namespace dbi {

PrintOperator::PrintOperator(unique_ptr<Operator> source, ostream& out, vector<harriet::Value>& globalRegister)
: globalRegister(globalRegister)
, source(move(source))
, out(out)
{
}

PrintOperator::~PrintOperator()
{
}

void PrintOperator::checkTypes() const throw(harriet::Exception)
{
}

void PrintOperator::dump(ostream& os) const
{
   os << "Print" << endl;
   source->dump(os, 3);
}

void PrintOperator::execute()
{
   // Calculate column widths
   uint32_t totalWidth = 1;
   vector<uint32_t> columnWidths;
   auto& signature = source->getSignature();
   for(auto& iter : signature.getAttributes()) {
      columnWidths.push_back(max((int)iter.name.size(), 10));
      totalWidth += columnWidths.back() + 3;
   }

   // Print header
   out << setfill(' ') << left << string(totalWidth, '-') << endl << "| ";
   for(uint32_t i=0; i<signature.getAttributes().size(); i++)
      out << setw(columnWidths[i]) << signature.getAttributes()[i].name << " | ";
   out << endl << string(totalWidth, '-') << endl;

   // Print content
   auto begin = chrono::high_resolution_clock::now();
   uint64_t tupleCount = 0;
   source->open();
   while(source->next()) {
      out << "| ";
      for(uint32_t i=0; i<signature.getAttributes().size(); i++)
         out << setw(columnWidths[i]) << globalRegister[signature.getAttributes()[i].index] << " | ";
      out << endl;
      tupleCount++;
   }
   auto end = chrono::high_resolution_clock::now();
   out << string(totalWidth, '-') << endl;
   out << "fetched " << tupleCount << " tuples in " << util::formatTime(chrono::duration_cast<chrono::nanoseconds>(end-begin), 3) << endl;

   source->close();
}

}
