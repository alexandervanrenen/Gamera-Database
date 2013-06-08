#include "SingleRecordOperator.hpp"
#include "segment_manager/Record.hpp"
#include "schema/RelationSchema.hpp"
#include "harriet/Expression.hpp"
#include <iostream>

using namespace std;

namespace dbi {

SingleRecordOperator::SingleRecordOperator(const std::vector<std::unique_ptr<harriet::Value>>& input, const RelationSchema& schema)
: state(kClosed)
, hasNext(false)
, schema(schema)
{
   for(auto& value : input)
      values.push_back(value->evaluate());
}

SingleRecordOperator::~SingleRecordOperator()
{
}

const RelationSchema& SingleRecordOperator::getSignatur() const
{
   return schema;
}

void SingleRecordOperator::open()
{
   assert(state == kClosed);
   hasNext = true;
   state = kOpen;
}

bool SingleRecordOperator::next()
{
   assert(state == kOpen);
   hasNext = false;
   return hasNext;
}

vector<unique_ptr<harriet::Value>> SingleRecordOperator::getOutput()
{
   vector<unique_ptr<harriet::Value>> result;
   for(auto& value : values)
      result.push_back(value->evaluate());
   return result;
}

void SingleRecordOperator::close()
{
   assert(state == kOpen);
   hasNext = false;
   state = kClosed;
}

}
