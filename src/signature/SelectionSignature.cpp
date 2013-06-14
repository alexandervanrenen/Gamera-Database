#include "SelectionSignature.hpp"
#include "schema/Common.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Environment.hpp"
#include "harriet/Value.hpp"
#include <sstream>
#include <algorithm>

using namespace std;

namespace dbi {

SelectionSignature::SelectionSignature(const Signature& source, std::unique_ptr<harriet::Expression> expression)
{
   // The input variables are all forwarded to the next operator
   for(auto& attribute : source.getAttributes())
      attributes.push_back(attribute);

   // Find free variables in the condition expression
   variableMapping = getFreeVariables(*expression);

   // Check expression type
   harriet::Environment env;
   for(auto& iter : variableMapping) {
      auto type = source.getAttributes()[iter.position].type;
      env.add(iter.name, type.createDefaultValue());
   }
   if(expression->evaluate(env)->type.type != harriet::VariableType::Type::TBool) {
      ostringstream os;
      expression->print(os);
      throw harriet::Exception{"Result type of: '" + os.str() + "' is not bool."};
   }

   // Optimize -- constant values. an expression that evaluates to a constant value (e.g. 2==3 is always false)
   if(variableMapping.size() == 0) {
      harriet::Environment env; // Empty environment for now
      selectionCondition = expression->evaluate(env);
      type = Type::kConstant;
      return;
   }

   // Optimize -- column-reference with constant. an expression with a single column access (s.a == 3)
   if(variableMapping.size()==1 && expression->isLogicOperator()) {
      // Acquire the constant subtree of the expression
      auto& rootOperator = reinterpret_cast<harriet::LogicOperator&>(*expression.get());
      unique_ptr<harriet::Expression>* constantSubTree = &rootOperator.lhs;
      unique_ptr<harriet::Expression>* columnRefSubTree = &rootOperator.rhs;
      if(getFreeVariables(**constantSubTree).size() != 0)
         swap(constantSubTree, columnRefSubTree);

      // Evaluate the constant subtree and replace it with the computed result
      harriet::Environment env; // Empty environment for now
      (*constantSubTree) = (*constantSubTree)->evaluate(env);

      // This optimization only works if the variable subtree is a singe variable
      if((*columnRefSubTree)->getExpressionType() == harriet::ExpressionType::TVariable) {
         selectionCondition = move(*constantSubTree);
         type = Type::kOneColumn;
      } else {
         selectionCondition = move(expression);
         type = Type::kComplex;
      }
      return;
   }

   // Optimize -- column-reference with column-reference
   if(variableMapping.size()==2 && expression->isLogicOperator()) {
      // Optimization only works if both children of the operator are variables => direct comparison
      auto& rootOperator = reinterpret_cast<harriet::LogicOperator&>(*expression.get());
      if(rootOperator.lhs->getExpressionType()==harriet::ExpressionType::TVariable && rootOperator.rhs->getExpressionType()==harriet::ExpressionType::TVariable) {
         type = Type::kTwoColumn;
      } else {
         type = Type::kComplex;
         selectionCondition = move(expression);
      }
      return;
   }

   type = Type::kComplex;
   selectionCondition = move(expression);
}

bool SelectionSignature::fullfillsPredicates(const vector<unique_ptr<harriet::Value>>& tuple)
{
   harriet::Environment env;
   if(type == Type::kConstant) // => Constant Value
      return reinterpret_cast<harriet::Value&>(*selectionCondition).data.vbool;

   if(type == Type::kOneColumn) // => a = 3
      return tuple[variableMapping[0].position]->computeEq(reinterpret_cast<harriet::Value&>(*selectionCondition)).data.vbool;

   if(type == Type::kTwoColumn) // => a = b
      return tuple[variableMapping[0].position]->computeEq(*tuple[variableMapping[1].position]).data.vbool;

   // => something wired
   for(auto& iter : variableMapping)
      env.add(iter.name, tuple[iter.position]->evaluate());
   return selectionCondition->evaluate(env)->data.vbool;
}

void SelectionSignature::dump(ostream& os) const
{
   Signature::dump(os);
   os << " opt: " << (int)type;
}

vector<SelectionSignature::VariableMapping> SelectionSignature::getFreeVariables(const harriet::Expression& expression) const
{
   vector<VariableMapping> result;
   vector<const harriet::Variable*> freeVariables = expression.getAllVariables();
   for(auto iter : freeVariables) {
      ColumnReference c(iter->getIdentifier());
      if(hasAttribute(c.tableQualifier, c.columnName)) {
         result.push_back(VariableMapping{iter->getIdentifier(), getAttributeIndex(c.tableQualifier, c.columnName)});
      } else {
         ostringstream os;
         dump(os);
         throw harriet::Exception{"unknown identifier: '" + iter->getIdentifier() + "' \ncandidates are: " + (os.str().size()==0?"<none>":os.str())};
      }
   }
   sort(result.begin(), result.end(), [](const VariableMapping& lhs, const VariableMapping& rhs){return lhs.name<rhs.name;});
   result.erase(unique(result.begin(), result.end(), [](const VariableMapping& lhs, const VariableMapping& rhs){return lhs.name==rhs.name;}), result.end());
   return result;
}

}
