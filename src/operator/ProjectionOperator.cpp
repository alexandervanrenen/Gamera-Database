#include "ProjectionOperator.hpp"
#include "harriet/Expression.hpp"
#include <iostream>

using namespace std;

namespace dbi {

ProjectionOperator::ProjectionOperator(std::unique_ptr<Operator> source, const vector<ColumnIdentifier>& projectedAttributes)
: source(move(source))
, state(kClosed)
, projectedAttributes(projectedAttributes)
, signature(this->source->getSignature().createProjectionSignature(projectedAttributes))
{
}

ProjectionOperator::~ProjectionOperator()
{
}

const Signature& ProjectionOperator::getSignature() const
{
   return signature;
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
   projection = source->getSignature().createProjection(projectedAttributes);
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
   auto tuple = source->getOutput();
   vector<unique_ptr<harriet::Value>> result;
   for(auto iter : projection)
      result.push_back(tuple[iter]->evaluate());
   return result;
}

void ProjectionOperator::close()
{
   assert(state == kOpen);
   source->close();
   state = kClosed;
}

}
