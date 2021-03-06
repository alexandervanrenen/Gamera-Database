#include "PrintOperator.hpp"
#include "harriet/ScriptLanguage.hpp"
#include "harriet/Value.hpp"
#include "ProjectionOperator.hpp"
#include "query/util/ColumnAccessInfo.hpp"
#include "query/util/Projection.hpp"
#include "query/util/GlobalRegister.hpp"
#include "util/Utility.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>

using namespace std;

namespace dbi {

PrintOperator::PrintOperator(unique_ptr<ProjectionOperator> source, qopt::GlobalRegister& globalRegister)
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
   for(auto& iter : source->getProjections())
      result.push_back(iter->alias);
   return result;
}

chrono::nanoseconds PrintOperator::getExecutionTime() const
{
   return executionTime;
}

void PrintOperator::execute()
{
   auto& projections = source->getProjections();

   // Print content
   auto begin = chrono::high_resolution_clock::now();
   source->open();
   while(source->next()) {
      result.push_back(vector<harriet::Value>());
      for(auto& projection : projections)
         result.back().emplace_back(globalRegister.getSlotValue(projection->resultRegisterSlot).createCopy());
   }
   auto end = chrono::high_resolution_clock::now();
   executionTime = chrono::duration_cast<chrono::nanoseconds>(end-begin);

   source->close();
}

}
