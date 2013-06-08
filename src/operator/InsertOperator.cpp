#include "InsertOperator.hpp"
#include "Operator.hpp"
#include "harriet/Expression.hpp"
#include <iostream>

using namespace std;

namespace dbi {

InsertOperator::InsertOperator(const RelationSchema& schema, unique_ptr<Operator> source, SPSegment& drain)
: schema(schema)
, source(move(source))
, drain(drain)
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
