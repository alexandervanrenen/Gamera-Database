#include "SingleRecordOperator.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Value.hpp"
#include <iostream>

using namespace std;

namespace dbi {

SingleRecordOperator::SingleRecordOperator(const std::vector<std::unique_ptr<harriet::Value>>& input)
: state(kClosed)
, hasNext(false)
, signature(input)
{
   for(auto& value : input)
      values.push_back(value->evaluate());
}

SingleRecordOperator::~SingleRecordOperator()
{
}

const Signature& SingleRecordOperator::getSignature() const
{
   return signature;
}

void SingleRecordOperator::checkTypes() const throw(harriet::Exception)
{
   return;
}

void SingleRecordOperator::dump(ostream& os, uint32_t lvl) const
{
   os << '|' << string(lvl, '.') << "Single Record " << "[";
   signature.dump(os);
   os << "]" << endl;
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
   bool result = hasNext;
   hasNext = false;
   return result;
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
