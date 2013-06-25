#include "ZeroRecordOperator.hpp"
#include "harriet/Value.hpp"
#include <cassert>
#include <iostream>

using namespace std;

namespace dbi {

ZeroRecordOperator::ZeroRecordOperator()
: state(kClosed)
{
}

ZeroRecordOperator::~ZeroRecordOperator()
{
}

void ZeroRecordOperator::dump(ostream& os, uint32_t lvl) const
{
   os << '|' << string(lvl, '.') << "Zero Record " << "[";
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
