#include "ZeroRecordOperator.hpp"
#include "harriet/Value.hpp"
#include "query/signature/ColumnSignature.hpp"
#include <cassert>
#include <iostream>

using namespace std;

namespace dbi {

ZeroRecordOperator::ZeroRecordOperator(set<qopt::ColumnAccessInfo>& projections)
: state(kClosed)
, signature(projections)
{
}

ZeroRecordOperator::~ZeroRecordOperator()
{
}

const Signature& ZeroRecordOperator::getSignature() const
{
   return signature;
}

void ZeroRecordOperator::dump(ostream& os, uint32_t lvl) const
{
   os << '|' << string(lvl, '.') << "Zero Record " << "[";
   signature.dump(os);
   os << "]" << endl;
}

void ZeroRecordOperator::open()
{
   assert(state == kClosed);
   state = kOpen;
}

bool ZeroRecordOperator::next()
{
   assert(state == kOpen);
   return false;
}

void ZeroRecordOperator::close()
{
   assert(state == kOpen);
   state = kClosed;
}

}
