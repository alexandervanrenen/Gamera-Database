#include "ProjectionOperator.hpp"
#include "query/util/ColumnAccessInfo.hpp"
#include <cassert>
#include <iostream>

using namespace std;

namespace dbi {

ProjectionOperator::ProjectionOperator(unique_ptr<Operator> source, const vector<qopt::ColumnAccessInfo>& projectedAttributes)
: source(move(source))
, state(kClosed)
, suppliedColumns(projectedAttributes)
{
   state = kClosed;
}

ProjectionOperator::~ProjectionOperator()
{
}

const vector<qopt::ColumnAccessInfo>& ProjectionOperator::getSuppliedColumns() const
{
   return suppliedColumns;
}

void ProjectionOperator::dump(ostream& os, uint32_t lvl) const
{
   os << "|" << string(lvl, '.') << "Projection ";
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
