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

PrintOperator::PrintOperator(unique_ptr<Operator> source, vector<harriet::Value>& globalRegister)
: globalRegister(globalRegister)
, source(move(source))
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

vector<vector<harriet::Value>>&& PrintOperator::getResult()
{
   return move(result);
}

vector<string> PrintOperator::getSuppliedColumns()
{
   vector<string> result;
   for(auto& iter : source->getSignature().getAttributes())
      result.push_back((iter.alias.size()!=0?(iter.alias+"."):"") + iter.name);
   return result;
}

chrono::nanoseconds PrintOperator::getExecutionTime() const
{
   return executionTime;
}

void PrintOperator::execute()
{
   // Print content
   auto& signature = source->getSignature();
   auto begin = chrono::high_resolution_clock::now();
   source->open();
   while(source->next()) {
      result.push_back(vector<harriet::Value>());
      for(uint32_t i=0; i<signature.getAttributes().size(); i++)
         result.back().emplace_back(globalRegister[signature.getAttributes()[i].index].createCopy());
   }
   auto end = chrono::high_resolution_clock::now();
   executionTime = chrono::duration_cast<chrono::nanoseconds>(end-begin);

   source->close();
}

}
