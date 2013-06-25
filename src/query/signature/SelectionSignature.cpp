#include "SelectionSignature.hpp"
#include "harriet/Environment.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Value.hpp"
#include "query/parser/Common.hpp"
#include "query/util/GlobalRegister.hpp"
#include "query/analyser/ExpressionOptimizer.hpp"
#include "util/Utility.hpp"
#include <algorithm>
#include <set>
#include <sstream>

using namespace std;

namespace dbi {

SelectionSignature::SelectionSignature(unique_ptr<qopt::Predicate> predicateIn, qopt::GlobalRegister& globalRegister)
: predicate(move(predicateIn))
{
   qgen::ExpressionOptimizer optimizer;
   harriet::Environment env;
   predicate->condition = optimizer.fullOptimization(*predicate->condition, globalRegister, env);
}

bool SelectionSignature::fullfillsPredicates() const
{
   harriet::Environment env;
   return predicate->condition->evaluate(env).data.vbool;
}

void SelectionSignature::dump(ostream& os) const
{
   os << predicate->prettyCondition;
}

}
