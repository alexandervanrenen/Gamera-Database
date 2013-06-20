#include "ProjectionOperator.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Value.hpp"
#include "harriet/Value.hpp"
#include "query_util/ColumnAccessInfo.hpp"
#include <iostream>

using namespace std;

namespace dbi {

ProjectionOperator::ProjectionOperator(unique_ptr<Operator> source, const vector<qopt::ColumnAccessInfo>& projectedAttributes, vector<harriet::Value>& globalRegister)
: source(move(source))
, state(kClosed)
, signature(projectedAttributes)
{
   signature.prepare(this->source->getSignature());
   state = kClosed;
}

ProjectionOperator::~ProjectionOperator()
{
}

const Signature& ProjectionOperator::getSignature() const
{
   return signature;
}

void ProjectionOperator::prepare(vector<harriet::Value>& globalRegister, const set<qopt::ColumnAccessInfo>& requiredColumns)
{
   throw;
   // assert(state == kUnprepared);
   // assert(requiredColumns.size()==0); // No one is above a projection

   // source->prepare(globalRegister, signature.getRequiredColumns());
   // signature.prepare(source->getSignature());
   // state = kClosed;
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

void ProjectionOperator::close()
{
   assert(state == kOpen);
   source->close();
   state = kClosed;
}

}
