#include "CrossProductOperator.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Value.hpp"
#include <iostream>

using namespace std;

namespace dbi {

CrossProductOperator::CrossProductOperator(unique_ptr<Operator> lhs, unique_ptr<Operator> rhs)
: lhs(move(lhs))
, rhs(move(rhs))
, lhsHasNext(false)
, state(kClosed)
{
}

CrossProductOperator::~CrossProductOperator()
{
}

void CrossProductOperator::dump(ostream& os, uint32_t lvl) const
{
   os << "|" << string(lvl, '.') << "CrossProduct ";
   os << endl << "|" << string(lvl+3, '.') << "lhs" << endl;
   lhs->dump(os, lvl+6);
   os << endl << "|" << string(lvl+3, '.') << "rhs" << endl;
   rhs->dump(os, lvl+6);
}

void CrossProductOperator::open()
{
   assert(state == kClosed);
   lhs->open();
   rhs->open();
   state = kOpen;

   lhsHasNext = lhs->next();
}

bool CrossProductOperator::next()
{
   assert(state == kOpen);

   while(lhsHasNext) {
      if(rhs->next())
         return true;
      rhs->close();
      rhs->open();
      lhsHasNext = lhs->next();
   }
   return false;
}

void CrossProductOperator::close()
{
   assert(state == kOpen);
   lhs->close();
   rhs->close();
   state = kClosed;
}

}
