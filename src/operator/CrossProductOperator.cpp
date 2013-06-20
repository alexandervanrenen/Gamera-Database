#include "CrossProductOperator.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Value.hpp"
#include <iostream>

using namespace std;

namespace dbi {

CrossProductOperator::CrossProductOperator(unique_ptr<Operator> lhs, unique_ptr<Operator> rhs)
: lhs(move(lhs))
, rhs(move(rhs))
, state(kClosed)
, signature(this->lhs->getSignature(), this->rhs->getSignature())
{
}

CrossProductOperator::~CrossProductOperator()
{
}

const Signature& CrossProductOperator::getSignature() const
{
   return signature;
}

void CrossProductOperator::prepare(vector<harriet::Value>& globalRegister, const set<qopt::ColumnAccessInfo>& requiredColumns)
{
   throw "not implemented";
}

void CrossProductOperator::dump(ostream& os, uint32_t lvl) const
{
   os << "|" << string(lvl, '.') << "CrossProduct ";
   signature.dump(os);
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

// vector<harriet::Value> CrossProductOperator::getOutput()
// {
//       auto lhsTuple = lhs->getOutput();
//       auto rhsTuple = rhs->getOutput();
//       lhsTuple.reserve(lhsTuple.size() + rhsTuple.size());
//       for(auto& iter : rhsTuple)
//          lhsTuple.emplace_back(move(iter));
//       return lhsTuple;
// }

void CrossProductOperator::close()
{
   assert(state == kOpen);
   lhs->close();
   rhs->close();
   state = kClosed;
}

}
