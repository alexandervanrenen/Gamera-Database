#include "SelectionSignature.hpp"
#include "schema/Common.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Environment.hpp"
#include <sstream>

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
   for(auto& attribute : source.getAttributes())
      attributes.push_back(attribute);

   // Find free variables
   variableMapping = getFreeVariables(*expression);

   // Optimize -- constant values
   if(variableMapping.size() == 0) {
      harriet::Environment env;
      this->expression = expression->evaluate(env);
      type = Type::TConstant;
      return;
   }

   // Optimize -- variable with constant (a == 3) (column is left)
   if(variableMapping.size() == 1) {
      harriet::BinaryOperator* op = reinterpret_cast<harriet::BinaryOperator*>(expression.get());
      uint32_t freeVariablesLeft = getFreeVariables(*op->lhs.get()).size();
      if(freeVariablesLeft == 0)
         swap(op->lhs, op->rhs);
      harriet::Environment env;
      op->rhs = op->rhs->evaluate(env);
      type = Type::TColumnAndConstant;
      if(op->lhs->getExpressionType() != harriet::ExpressionType::TVariable)
         throw;
      this->expression = move(op->rhs);
      return;
   }

   this->expression = move(expression);
}

bool SelectionSignature::fullfillsPredicates(const vector<unique_ptr<harriet::Value>>& tuple)
{
   harriet::Environment env;
   if(type == Type::TConstant)
      return reinterpret_cast<harriet::BoolValue&>(*expression->evaluate(env)).result;

   if(type == Type::TColumnAndConstant)
      return reinterpret_cast<harriet::BoolValue&>(*tuple[variableMapping[0].position]->computeEq(reinterpret_cast<harriet::Value&>(*expression))).result;

   throw;
   for(auto& iter : variableMapping)
      env.add(iter.name, tuple[iter.position]->evaluate());
   auto result = expression->evaluate(env);
   return reinterpret_cast<harriet::BoolValue&>(*result).result;
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
   return result;
}

}
