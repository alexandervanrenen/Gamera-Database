#include "InsertOperator.hpp"
#include "Operator.hpp"
#include "harriet/Expression.hpp"
#include "segment_manager/SPSegment.hpp"
#include <iostream>

using namespace std;

namespace dbi {

InsertOperator::InsertOperator(unique_ptr<Operator> source, SPSegment& target, const RelationSchema& targetSchema)
: source(move(source))
, target(target)
, targetSchema(targetSchema)
{
}

InsertOperator::~InsertOperator()
{
}

void InsertOperator::execute()
{
   auto& schema = source->getSignature();
   source->open();
   while(source->next()) {
      auto result = source->getOutput();
      target.insert(targetSchema.tupleToRecord(result));
   }
}

}
