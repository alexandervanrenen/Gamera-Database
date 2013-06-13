#include "SingleRecordOperator.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Value.hpp"
#include <iostream>

using namespace std;

namespace dbi {

SingleRecordOperator::SingleRecordOperator(std::vector<harriet::Value>&& input)
: state(kClosed)
, hasNext(false)
, signature(input)
, tuple(move(input))
{
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

vector<harriet::Value> SingleRecordOperator::getOutput()
{
   vector<harriet::Value> result;
   for(auto& value : tuple)
      result.push_back(move(*value.evaluate())); // AAA hack
   return result;
}

void SingleRecordOperator::close()
{
   assert(state == kOpen);
   hasNext = false;
   state = kClosed;
}

}
