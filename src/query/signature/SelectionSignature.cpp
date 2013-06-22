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

SelectionSignature::SelectionSignature(const Signature& source, unique_ptr<qopt::Predicate> predicateIn)
: predicate(move(predicateIn))
{
   // The input variables are all forwarded to the next operator
   for(auto& attribute : source.getAttributes())
      attributes.push_back(attribute);

   // // Find free variables in the condition expression
   variableMapping = getFreeVariables(predicate->condition);

   // Optimize -- constant values. an expression that evaluates to a constant value (e.g. 2==3 is always false)
   if(variableMapping.size() == 0) {
      harriet::Environment env; // Empty environment for now
      predicate->condition = util::make_unique<harriet::ValueExpression>(predicate->condition->evaluate(env));
      type = Type::kConstant;
      return;
   }

   // Optimize -- column-reference with constant. an expression with a single column access (s.a == 3)
   if(variableMapping.size()==1 && predicate->condition->isLogicOperator()) {
      // Acquire the constant subtree of the expression
      auto& rootOperator = reinterpret_cast<harriet::LogicOperator&>(*predicate->condition.get());
      unique_ptr<harriet::Expression>* constantSubTree = &rootOperator.lhs;
      unique_ptr<harriet::Expression>* columnRefSubTree = &rootOperator.rhs;
      if((*constantSubTree)->getAllVariables(constantSubTree).size() != 0)
         swap(constantSubTree, columnRefSubTree);

      // Evaluate the constant subtree and replace it with the computed result
      harriet::Environment env; // Empty environment for now
      (*constantSubTree) = util::make_unique<harriet::ValueExpression>((*constantSubTree)->evaluate(env));

      // This optimization only works if the variable subtree is a singe variable
      if((*columnRefSubTree)->getExpressionType() == harriet::ExpressionType::TVariable) {
         predicate->condition = move(*constantSubTree);
         type = Type::kOneColumn;
      } else {
         predicate->condition = move(predicate->condition);
         type = Type::kComplex;
      }
      return;
   }

   // Optimize -- column-reference with column-reference
   if(variableMapping.size()==2 && predicate->condition->isLogicOperator()) {
      // Optimization only works if both children of the operator are variables => direct comparison
      auto& rootOperator = reinterpret_cast<harriet::LogicOperator&>(*predicate->condition.get());
      predicate->condition = move(predicate->condition);
      if(rootOperator.lhs->getExpressionType()==harriet::ExpressionType::TVariable && rootOperator.rhs->getExpressionType()==harriet::ExpressionType::TVariable)
         type = Type::kTwoColumn; else
         type = Type::kComplex;
      return;
   }

   type = Type::kComplex;
}

bool SelectionSignature::fullfillsPredicates(const vector<harriet::Value>& tuple) const
{
   if(type == Type::kConstant) // => Constant Value
      return reinterpret_cast<harriet::Value&>(*predicate->condition).data.vbool;

   if(type == Type::kOneColumn) // => a = 3
      return tuple[variableMapping[0].position].computeEq(reinterpret_cast<harriet::ValueExpression&>(*predicate->condition).value).data.vbool;

   if(type == Type::kTwoColumn) // => a = b
      return tuple[variableMapping[0].position].computeEq(tuple[variableMapping[1].position]).data.vbool;

   // => something wired
   harriet::Environment env;
   for(auto& iter : variableMapping)
      env.add(iter.name, tuple[iter.position].createCopy());
   return predicate->condition->evaluate(env).data.vbool;
}

void SelectionSignature::dump(ostream& os) const
{
   os << "opt: " << (int)type;
   predicate->condition->print(os);
   Signature::dump(os);
}

vector<SelectionSignature::VariableMapping> SelectionSignature::getFreeVariables(unique_ptr<harriet::Expression>& expression) const
{
   vector<VariableMapping> result;
   vector<unique_ptr<harriet::Expression>*> freeVariables = expression->getAllVariables(&expression);
   for(auto iter : freeVariables) {
      ColumnReference c(reinterpret_cast<harriet::Variable&>(**iter).getIdentifier());
      if(hasAttribute(c.tableQualifier, c.columnName)) {
         result.push_back(VariableMapping{reinterpret_cast<harriet::Variable&>(**iter).getIdentifier(), getAttribute(c.tableQualifier, c.columnName).index});
      } else {
         ostringstream os;
         dump(os);
         throw harriet::Exception{"unknown identifier: '" + reinterpret_cast<harriet::Variable&>(**iter).getIdentifier() + "' \ncandidates are: " + (os.str().size()==0?"<none>":os.str())};
      }
   }
   sort(result.begin(), result.end(), [](const VariableMapping& lhs, const VariableMapping& rhs){return lhs.name<rhs.name;});
   result.erase(unique(result.begin(), result.end(), [](const VariableMapping& lhs, const VariableMapping& rhs){return lhs.name==rhs.name;}), result.end());
   return result;
}

}
