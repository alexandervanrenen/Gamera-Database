#include "InsertOperator.hpp"
#include "Operator.hpp"
#include "harriet/Expression.hpp"
#include <iostream>

using namespace std;

namespace dbi {

InsertOperator::InsertOperator(unique_ptr<Operator> source, SPSegment& target)
: source(move(source))
, target(target)
{
}

InsertOperator::~InsertOperator()
{
}

void InsertOperator::execute()
{
   source->open();
   while(source->next()) {
      auto result = source->getOutput();
      for(auto& iter : result)
         cout << *iter << endl;
   }
}

}
