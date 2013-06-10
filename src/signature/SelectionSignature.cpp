#include "SelectionSignature.hpp"
#include "schema/Common.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Environment.hpp"
#include <sstream>
#include <algorithm>

using namespace std;

namespace dbi {

namespace {
   ColumnIdentifier toColumnIdentifier(const string& str)
   {
      ColumnIdentifier id;
      size_t splitPos = str.find('.');
      id.tableIdentifier = str.substr(0, splitPos);
      id.columnIdentifier = (splitPos==string::npos?"":str.substr(splitPos+1, str.size()));
      return id;
   }
}

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
      env.add(iter.name, harriet::createDefaultValue(type));
   }
   if(expression->evaluate(env)->getResultType() != harriet::VariableType::TBool) {
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

   // Optimize -- variable with constant. an expression with a single column access (s.a == 3)
   if(variableMapping.size()==1 && expression->isLogicOperator()) {
      // Acquire the constant subtree of the expression
      auto& rootOperator = reinterpret_cast<harriet::LogicOperator&>(*expression.get());
      unique_ptr<harriet::Expression>* constantSubTree = &rootOperator.lhs;
      unique_ptr<harriet::Expression>* variableSubTree = &rootOperator.rhs;
      if(getFreeVariables(**constantSubTree).size() != 0)
         swap(constantSubTree, variableSubTree);

      // Evaluate the constant subtree and replace it with the computed result
      harriet::Environment env; // Empty environment for now
      (*constantSubTree) = (*constantSubTree)->evaluate(env);

      // This optimization only works if the variable subtree is a singe variable
      if((*variableSubTree)->getExpressionType() == harriet::ExpressionType::TVariable) {
         selectionCondition = move(*constantSubTree);
         type = Type::kOneColumn;
      } else {
         selectionCondition = move(expression);
         type = Type::kComplex;
      }
      return;
   }

   type = Type::kComplex;
   selectionCondition = move(expression);
}

bool SelectionSignature::fullfillsPredicates(const vector<unique_ptr<harriet::Value>>& tuple)
{
   harriet::Environment env;
   if(type == Type::kConstant) // => Constant
      return reinterpret_cast<harriet::BoolValue&>(*selectionCondition->evaluate(env)).result;

   if(type == Type::kOneColumn) // => Constant
      return reinterpret_cast<harriet::BoolValue&>(*tuple[variableMapping[0].position]->computeEq(reinterpret_cast<harriet::Value&>(*selectionCondition))).result;

   for(auto& iter : variableMapping)
      env.add(iter.name, tuple[iter.position]->evaluate());
   auto result = selectionCondition->evaluate(env);
   return reinterpret_cast<harriet::BoolValue&>(*result).result;
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
      ColumnIdentifier c = toColumnIdentifier(iter->getIdentifier());
      if(hasAttribute(c.tableIdentifier, c.columnIdentifier)) {
         result.push_back(VariableMapping{iter->getIdentifier(), getAttributeIndex(c.tableIdentifier, c.columnIdentifier)});
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
