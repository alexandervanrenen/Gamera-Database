#include "SelectionOperator.hpp"
#include "harriet/Expression.hpp"
#include <iostream>

using namespace std;

namespace dbi {

SelectionOperator::SelectionOperator(std::unique_ptr<Operator> source, unique_ptr<harriet::Expression> expression)
: source(move(source))
, state(kClosed)
, signature(this->source->getSignature(), move(expression))
{
}

SelectionOperator::~SelectionOperator()
{
}

const Signature& SelectionOperator::getSignature() const
{
   return signature;
}

void SelectionOperator::checkTypes() const throw(harriet::Exception)
{
   return;
}

void SelectionOperator::dump(ostream& os, uint32_t lvl) const
{
   os << "|" << string(lvl, '.') << "Selection [";
   signature.dump(os);
   os << "]" << endl;
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
      tuple = source->getOutput();
      if(signature.fullfillsPredicates(tuple))
         return true;
   }
   return false;
}

vector<unique_ptr<harriet::Value>> SelectionOperator::getOutput()
{
   return move(tuple);
}

void SelectionOperator::close()
{
   assert(state == kOpen);
   source->close();
   state = kClosed;
}

}
