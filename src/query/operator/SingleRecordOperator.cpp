#include "SingleRecordOperator.hpp"
#include "harriet/Value.hpp"
#include "query/util/GlobalRegister.hpp"
#include "query/util/ColumnAccessInfo.hpp"
#include <cassert>
#include <iostream>

using namespace std;

namespace dbi {
SingleRecordOperator::SingleRecordOperator(vector<harriet::Value>&& input, qopt::GlobalRegister& globalRegister)
: state(kClosed)
, hasNext(false)
, tuple(move(input))
, globalRegister(globalRegister)
{
   assert(globalRegister.size() == tuple.size());
}

SingleRecordOperator::~SingleRecordOperator()
{
}

void SingleRecordOperator::dump(ostream& os, uint32_t lvl) const
{
   os << '|' << string(lvl, '.') << "Single Record " << "[";
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
      globalRegister.getSlotValue(i) = tuple[i].createCopy();
   return result;
}

void SingleRecordOperator::close()
{
   assert(state == kOpen);
   hasNext = false;
   state = kClosed;
}

}
