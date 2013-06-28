#include "SelectionOperator.hpp"
#include "query/util/Predicate.hpp"
#include <iostream>
#include <cassert>

using namespace std;

namespace dbi {

SelectionOperator::SelectionOperator(unique_ptr<Operator> source, unique_ptr<qopt::Predicate> predicate, qopt::GlobalRegister& globalRegister)
: source(move(source))
, state(kClosed)
, signature(move(predicate), globalRegister)
{
}

SelectionOperator::~SelectionOperator()
{
}

void SelectionOperator::dump(ostream& os, uint32_t lvl) const
{
   os << "|" << string(lvl, '.') << "Selection ";
   signature.dump(os);
   os << endl;
   source->dump(os, lvl+3);
}

void SelectionOperator::open()
{
   assert(state == kClosed);
   source->open();
   state = kOpen;
}

bool SelectionOperator::next()
{
   assert(state == kOpen);
   while(source->next()) {
      if(signature.fullfillsPredicates())
         return true;
   }
   return false;
}

void SelectionOperator::close()
{
   assert(state == kOpen);
   source->close();
   state = kClosed;
}

}
