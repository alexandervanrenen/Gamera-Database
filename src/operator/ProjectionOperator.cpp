#include "ProjectionOperator.hpp"
#include "harriet/Expression.hpp"
#include <iostream>

using namespace std;

namespace dbi {

ProjectionOperator::ProjectionOperator(std::unique_ptr<Operator> source, const std::vector<ColumnIdentifier>& columns)
: source(move(source))
, state(kClosed)
{
}

ProjectionOperator::~ProjectionOperator()
{
}

const Signature& ProjectionOperator::getSignature() const
{
   return source->getSignature();
}

void ProjectionOperator::checkTypes() const throw(harriet::Exception)
{
   return;
}

void ProjectionOperator::dump(ostream& os, uint32_t lvl) const
{
   os << "|" << string(lvl, '.') << "Projection " << endl;
   source->dump(os, lvl+3);
}

void ProjectionOperator::open()
{
   assert(state == kClosed);
   source->open();
   state = kOpen;
}

bool ProjectionOperator::next()
{
   assert(state == kOpen);
   return source->next();
}

vector<unique_ptr<harriet::Value>> ProjectionOperator::getOutput()
{
   return source->getOutput();
}

void ProjectionOperator::close()
{
   assert(state == kOpen);
   source->close();
   state = kClosed;
}

}
