#include "ProjectionOperator.hpp"
#include "query/util/ColumnAccessInfo.hpp"
#include "query/util/Projection.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Environment.hpp"
#include "query/util/GlobalRegister.hpp"
#include "query/analyser/ExpressionOptimizer.hpp"
#include <cassert>
#include <iostream>

using namespace std;

namespace dbi {

ProjectionOperator::ProjectionOperator(unique_ptr<Operator> source, vector<unique_ptr<qopt::Projection>>&& projectionsIn, qopt::GlobalRegister& globalRegister)
: source(move(source))
, state(kClosed)
, projections(move(projectionsIn))
, globalRegister(globalRegister)
{
   state = kClosed;

   // Replace the column references in the condition with pointer to the global register
   qgen::ExpressionOptimizer optimizer;
   harriet::Environment env;
   for(auto& projection : projections)
      projection->expression = optimizer.fullOptimization(*projection->expression, globalRegister, env);
}

ProjectionOperator::~ProjectionOperator()
{
}

const vector<unique_ptr<qopt::Projection>>& ProjectionOperator::getProjections() const
{
   return projections;
}

void ProjectionOperator::dump(ostream& os, uint32_t lvl) const
{
   os << "|" << string(lvl, '.') << "Projection ";
   for(auto& iter : projections)
      os << iter->alias << " (" << iter->resultRegisterSlot << ") ";
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
   if(!source->next())
      return false;

   // Do the mapping
   harriet::Environment env;
   for(auto& iter : projections)
      globalRegister.getSlotValue(iter->resultRegisterSlot) = iter->expression->evaluate(env);
   return true;
}

void ProjectionOperator::close()
{
   assert(state == kOpen);
   source->close();
   state = kClosed;
}

}
