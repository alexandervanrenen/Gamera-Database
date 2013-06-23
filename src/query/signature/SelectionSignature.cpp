#include "SelectionSignature.hpp"
#include "harriet/Environment.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Value.hpp"
#include "query/parser/Common.hpp"
#include "query/signature/ColumnSignature.hpp"
#include "util/Utility.hpp"
#include <algorithm>
#include <set>
#include <sstream>

using namespace std;

namespace dbi {

SelectionSignature::SelectionSignature(const Signature& source, unique_ptr<qopt::Predicate> predicateIn, vector<harriet::Value>& golbalRegister)
: predicate(move(predicateIn))
, golbalRegister(golbalRegister)
{
   // The input variables are all forwarded to the next operator
   for(auto& attribute : source.getAttributes())
      attributes.push_back(attribute);

   // Replace the column references in the condition with pointer to the global register
   auto condition = predicate->condition.release();
   vector<harriet::Expression**> freeVariables = condition->getAllVariables(&condition);
   for(auto iter : freeVariables) {
      ColumnReference c((*iter)->identifier);
      if(hasAttribute(c.tableQualifier, c.columnName)) {
         delete *iter;
         *iter = harriet::Expression::createValueReferenceExpression(&golbalRegister[getAttribute(c.tableQualifier, c.columnName).index]).release();
      } else {
         ostringstream os;
         dump(os);
         throw harriet::Exception{"unknown identifier: '" + (*iter)->identifier + "' \ncandidates are: " + (os.str().size()==0?"<none>":os.str())};
      }
   }
   predicate->condition.reset(condition);
}

bool SelectionSignature::fullfillsPredicates(const vector<harriet::Value>& tuple) const
{
   harriet::Environment env;
   return predicate->condition->evaluate(env).data.vbool;
}

void SelectionSignature::dump(ostream& os) const
{
   predicate->condition->print(os);
   Signature::dump(os);
}

}
