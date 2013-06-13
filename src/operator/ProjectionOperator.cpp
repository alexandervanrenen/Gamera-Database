#include "ProjectionOperator.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Value.hpp"
#include <iostream>

using namespace std;

namespace dbi {

ProjectionOperator::ProjectionOperator(std::unique_ptr<Operator> source, const vector<ColumnReference>& projectedAttributes)
: source(move(source))
, state(kClosed)
, signature(this->source->getSignature(), projectedAttributes)
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
   os << "|" << string(lvl, '.') << "Projection ";
   signature.dump(os);
   os << endl;
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

vector<harriet::Value> ProjectionOperator::getOutput()
{
   auto tuple = source->getOutput();
   vector<harriet::Value> result;
   for(auto iter : signature.getProjection())
      result.push_back(move(tuple[iter]));
   return result;
}

void ProjectionOperator::close()
{
   assert(state == kOpen);
   source->close();
   state = kClosed;
}

}
