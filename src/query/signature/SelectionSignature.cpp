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

   vector<unique_ptr<harriet::Expression>*> freeVariables = predicate->condition->getAllVariables(&predicate->condition);
   for(auto iter : freeVariables) {
      ColumnReference c(reinterpret_cast<harriet::Variable&>(**iter).getIdentifier());
      if(hasAttribute(c.tableQualifier, c.columnName)) {
         *iter = util::make_unique<harriet::ValueReferenceExpression>(&golbalRegister[getAttribute(c.tableQualifier, c.columnName).index]);
      } else {
         ostringstream os;
         dump(os);
         throw harriet::Exception{"unknown identifier: '" + reinterpret_cast<harriet::Variable&>(**iter).getIdentifier() + "' \ncandidates are: " + (os.str().size()==0?"<none>":os.str())};
      }
   }
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
