#include "SingleRecordOperator.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Value.hpp"
#include <iostream>
#include <cassert>

using namespace std;

namespace dbi {

SingleRecordOperator::SingleRecordOperator(vector<harriet::Value>&& input, vector<harriet::Value>& globalRegister)
: state(kClosed)
, hasNext(false)
, signature(input)
, tuple(move(input))
, globalRegister(globalRegister)
{
   registerOffset = globalRegister.size();
   for(auto iter : signature.getAttributes())
      globalRegister.emplace_back(harriet::Value::createDefault(harriet::VariableType()));
}

SingleRecordOperator::~SingleRecordOperator()
{
}

const Signature& SingleRecordOperator::getSignature() const
{
   assert(state != kUnprepared);
   return signature;
}

void SingleRecordOperator::prepare(vector<harriet::Value>& globalRegister, const set<qopt::ColumnAccessInfo>& requiredColumns)
{
   throw;
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
   for(uint32_t i=0; i<tuple.size(); i++)
      globalRegister[registerOffset+i] = tuple[i].createCopy();
   return result;
}

void SingleRecordOperator::close()
{
   assert(state == kOpen);
   hasNext = false;
   state = kClosed;
}

}
