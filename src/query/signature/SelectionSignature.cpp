#include "SelectionSignature.hpp"
#include "harriet/Environment.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Value.hpp"
#include "query/parser/Common.hpp"
#include "query/util/GlobalRegister.hpp"
#include "util/Utility.hpp"
#include <algorithm>
#include <set>
#include <sstream>

using namespace std;

namespace dbi {

SelectionSignature::SelectionSignature(unique_ptr<qopt::Predicate> predicateIn, qopt::GlobalRegister& globalRegister)
: predicate(move(predicateIn))
{
   // Replace the column references in the condition with pointer to the global register
   auto condition = predicate->condition.release();
   vector<harriet::Expression**> freeVariables = condition->getAllVariables(&condition);
   for(auto iter : freeVariables) {
      ColumnReference c((*iter)->identifier);
      if(globalRegister.hasColumn(c.tableQualifier, c.columnName)) {
         delete *iter;
         uint32_t columnIndex = globalRegister.getColumnIndex(c.tableQualifier, c.columnName);
         *iter = harriet::Expression::createValueReferenceExpression(&globalRegister.getValue(columnIndex)).release();
      } else {
         predicate->condition.reset(condition);
         ostringstream os;
         dump(os);
         throw harriet::Exception{"unknown identifier: '" + (*iter)->identifier + "' \ncandidates are: " + (os.str().size()==0?"<none>":os.str())};
      }
   }
   predicate->condition.reset(condition);
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
