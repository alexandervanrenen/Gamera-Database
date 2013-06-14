#include "Expression.hpp"
#include "Utility.hpp"
#include "Environment.hpp"
#include "Function.hpp"
#include "Value.hpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <stack>
#include <cassert>
#include <cmath>
//---------------------------------------------------------------------------
// Harriet Script Language
// Copyright (c) 2013 Alexander van Renen (alexandervanrenen@gmail.com)
// See the file LICENSE.txt for copying permission.
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace harriet {
//---------------------------------------------------------------------------
bool Expression::isLogicOperator() const
{
   ExpressionType t = getExpressionType();
   return t==ExpressionType::TAndOperator || t==ExpressionType::TOrOperator || t==ExpressionType::TGreaterOperator || t==ExpressionType::TLessOperator || t==ExpressionType::TGreaterEqualOperator || t==ExpressionType::TLessEqualOperator || t==ExpressionType::TEqualOperator || t==ExpressionType::TNotEqualOperator;
}
//---------------------------------------------------------------------------
void Variable::print(ostream& stream) const
{
   stream << identifier;
}
//---------------------------------------------------------------------------
unique_ptr<Value> Variable::evaluate(Environment& environment) const
{
   const Value& result = environment.read(identifier);
   return result.evaluate();
}
//---------------------------------------------------------------------------
void UnaryOperator::print(ostream& stream) const
{
   stream << " ( " << getSign();
   child->print(stream);
   stream << " ) ";
}
//---------------------------------------------------------------------------
void UnaryOperator::addChild(unique_ptr<Expression> child)
{
   this->child = ::move(child);
}
//---------------------------------------------------------------------------
unique_ptr<Value> UnaryMinusOperator::evaluate(Environment&) const
{
   // return child->evaluate(environment)->computeInv();
   return nullptr;
}
//---------------------------------------------------------------------------
unique_ptr<Value> NotOperator::evaluate(Environment&) const
{
   // return child->evaluate(environment)->computeNot();
   return nullptr;
}
//---------------------------------------------------------------------------
void BinaryOperator::addChildren(unique_ptr<Expression> lhsChild, unique_ptr<Expression> rhsChild)
{
   assert(lhs==nullptr && rhs==nullptr);
   lhs = ::move(lhsChild);
   rhs = ::move(rhsChild);
}
//---------------------------------------------------------------------------
void BinaryOperator::print(ostream& stream) const
{
   stream << " ( ";
   lhs->print(stream);
   stream << getSign();
   rhs->print(stream);
   stream << " ) ";
}
//---------------------------------------------------------------------------
unique_ptr<Value> AssignmentOperator::evaluate(Environment& environment) const
{
   if(lhs->getExpressionType() != ExpressionType::TVariable)
      throw harriet::Exception("need variable as left hand side of assignment operator");

   environment.update(reinterpret_cast<Variable*>(lhs.get())->getIdentifier(), rhs->evaluate(environment));
   return lhs->evaluate(environment);
}
//---------------------------------------------------------------------------
unique_ptr<Value> PlusOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment)->computeAdd(*rhs->evaluate(environment)).evaluate();
}
//---------------------------------------------------------------------------
unique_ptr<Value> MinusOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment)->computeSub(*rhs->evaluate(environment)).evaluate();
}
//---------------------------------------------------------------------------
unique_ptr<Value> MultiplicationOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeMul(*rhs->evaluate(environment));
   return nullptr;
}
//---------------------------------------------------------------------------
unique_ptr<Value> DivisionOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeDiv(*rhs->evaluate(environment));
   return nullptr;
}
//---------------------------------------------------------------------------
unique_ptr<Value> ModuloOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeMod(*rhs->evaluate(environment));
   return nullptr;
}
//---------------------------------------------------------------------------
unique_ptr<Value> ExponentiationOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeExp(*rhs->evaluate(environment));
   return nullptr;
}
//---------------------------------------------------------------------------
unique_ptr<Value> AndOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeAnd(*rhs->evaluate(environment));
   return nullptr;
}
//---------------------------------------------------------------------------
unique_ptr<Value> OrOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeOr (*rhs->evaluate(environment));
   return nullptr;
}
//---------------------------------------------------------------------------
unique_ptr<Value> GreaterOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeGt (*rhs->evaluate(environment));
   return nullptr;
}
//---------------------------------------------------------------------------
unique_ptr<Value> LessOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeLt (*rhs->evaluate(environment));
   return nullptr;
}
//---------------------------------------------------------------------------
unique_ptr<Value> GreaterEqualOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeGeq(*rhs->evaluate(environment));
   return nullptr;
}
//---------------------------------------------------------------------------
unique_ptr<Value> LessEqualOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeLeq(*rhs->evaluate(environment));
   return nullptr;
}
//---------------------------------------------------------------------------
unique_ptr<Value> EqualOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment)->computeEq(*rhs->evaluate(environment)).evaluate();
}
//---------------------------------------------------------------------------
unique_ptr<Value> NotEqualOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeNeq(*rhs->evaluate(environment));
   return nullptr;
}
//---------------------------------------------------------------------------
FunctionOperator::FunctionOperator(const string& functionName, uint32_t functionIdentifier, vector<unique_ptr<Expression>>& arguments)
: functionName(functionName)
, functionIdentifier(functionIdentifier)
, arguments(::move(arguments))
{
}
//---------------------------------------------------------------------------
unique_ptr<Value> FunctionOperator::evaluate(Environment& environment) const
{
   // build arguments
   vector<unique_ptr<Value>> evaluetedArguments;
   auto function = environment.getFunction(functionIdentifier);
   for(uint32_t i=0; i<arguments.size(); i++) {
      auto result = arguments[i]->evaluate(environment);
      if(result->type != function->getArgumentType(i))
         throw harriet::Exception{"type missmatch in function '" + function->getName() + "' for argument '" + to_string(i) + "' unable to convert '" + result->type.str() + "' to '" + function->getArgumentType(i).str() + "'"};
      evaluetedArguments.push_back(::move(result));
   }

   // call function
   return function->execute(evaluetedArguments, environment);
}
//---------------------------------------------------------------------------
void FunctionOperator::print(ostream& stream) const
{
   stream << functionName << " id:" << functionIdentifier << endl;
}
//---------------------------------------------------------------------------
} // end of namespace harriet
//---------------------------------------------------------------------------
