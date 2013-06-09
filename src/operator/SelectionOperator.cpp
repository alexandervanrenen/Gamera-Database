#include "SelectionOperator.hpp"
#include "harriet/Expression.hpp"
#include <iostream>

using namespace std;

namespace dbi {

SelectionOperator::SelectionOperator(std::unique_ptr<Operator> source, const Predicate& predicate)
: source(move(source))
, state(kClosed)
, signature(this->source->getSignature(), predicate)
{
}

SelectionOperator::~SelectionOperator()
{
}

const Signature& SelectionOperator::getSignature() const
{
   return source->getSignature();
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
   return source->next();
}

vector<unique_ptr<harriet::Value>> SelectionOperator::getOutput()
{
   auto tuple = source->getOutput();
   return tuple;
}

void SelectionOperator::close()
{
   assert(state == kOpen);
   source->close();
   state = kClosed;
}

}
