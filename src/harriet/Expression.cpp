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
Value Variable::evaluate(Environment& environment) const
{
   const Value& result = environment.read(identifier);
   return result.createCopy();
}
//---------------------------------------------------------------------------
void ValueExpression::print(ostream& stream) const
{
   stream << value;
}
//---------------------------------------------------------------------------
Value ValueExpression::evaluate(Environment&) const
{
   return value.createCopy();
}
//---------------------------------------------------------------------------
vector<unique_ptr<Expression>*> UnaryOperator::getAllVariables(unique_ptr<Expression>*)
{
   return child->getAllVariables(&child);
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
Value UnaryMinusOperator::evaluate(Environment&) const
{
   // return child->evaluate(environment)->computeInv();
   throw;
}
//---------------------------------------------------------------------------
Value NotOperator::evaluate(Environment&) const
{
   // return child->evaluate(environment)->computeNot();
   throw;
}
//---------------------------------------------------------------------------
vector<unique_ptr<Expression>*> BinaryOperator::getAllVariables(unique_ptr<Expression>*)
{
   vector<unique_ptr<Expression>*> result;
   auto lhsVariables = lhs->getAllVariables(&lhs);
   for(auto var : lhsVariables)
      result.push_back(var);
   auto rhsVariables = rhs->getAllVariables(&rhs);
   for(auto var : rhsVariables)
      result.push_back(var);
   return result;
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
Value AssignmentOperator::evaluate(Environment&) const
{
   if(lhs->getExpressionType() != ExpressionType::TVariable)
      throw harriet::Exception("need variable as left hand side of assignment operator");
throw;
   // environment.update(reinterpret_cast<Variable*>(lhs.get())->getIdentifier(), rhs->evaluate(environment));
   // return lhs->evaluate(environment);
}
//---------------------------------------------------------------------------
Value PlusOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment).computeAdd(rhs->evaluate(environment));
}
//---------------------------------------------------------------------------
Value MinusOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment).computeSub(rhs->evaluate(environment));
}
//---------------------------------------------------------------------------
Value MultiplicationOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeMul(*rhs->evaluate(environment));
   throw;
}
//---------------------------------------------------------------------------
Value DivisionOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeDiv(*rhs->evaluate(environment));
   throw;
}
//---------------------------------------------------------------------------
Value ModuloOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeMod(*rhs->evaluate(environment));
   throw;
}
//---------------------------------------------------------------------------
Value ExponentiationOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeExp(*rhs->evaluate(environment));
   throw;
}
//---------------------------------------------------------------------------
AndOperator::AndOperator(unique_ptr<Expression> lhs, unique_ptr<Expression> rhs) {
   this->lhs = move(lhs);
   this->rhs = move(rhs);
}
//---------------------------------------------------------------------------
Value AndOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment).computeAnd(rhs->evaluate(environment));
}
//---------------------------------------------------------------------------
Value OrOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeOr (*rhs->evaluate(environment));
   throw;
}
//---------------------------------------------------------------------------
Value GreaterOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeGt (*rhs->evaluate(environment));
   throw;
}
//---------------------------------------------------------------------------
Value LessOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeLt (*rhs->evaluate(environment));
   throw;
}
//---------------------------------------------------------------------------
Value GreaterEqualOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeGeq(*rhs->evaluate(environment));
   throw;
}
//---------------------------------------------------------------------------
Value LessEqualOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeLeq(*rhs->evaluate(environment));
   throw;
}
//---------------------------------------------------------------------------
Value EqualOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment).computeEq(rhs->evaluate(environment));
}
//---------------------------------------------------------------------------
Value NotEqualOperator::evaluate(Environment&) const
{
   // return lhs->evaluate(environment)->computeNeq(*rhs->evaluate(environment));
   throw;
}
//---------------------------------------------------------------------------
FunctionOperator::FunctionOperator(const string& functionName, uint32_t functionIdentifier, vector<unique_ptr<Expression>>& arguments)
: functionName(functionName)
, functionIdentifier(functionIdentifier)
, arguments(::move(arguments))
{
}
//---------------------------------------------------------------------------
Value FunctionOperator::evaluate(Environment&) const
{
   throw;
   // // build arguments
   // vector<unique_ptr<Value>> evaluetedArguments;
   // auto function = environment.getFunction(functionIdentifier);
   // for(uint32_t i=0; i<arguments.size(); i++) {
   //    auto result = arguments[i]->evaluate(environment);
   //    if(result->type != function->getArgumentType(i))
   //       throw harriet::Exception{"type missmatch in function '" + function->getName() + "' for argument '" + to_string(i) + "' unable to convert '" + result->type.str() + "' to '" + function->getArgumentType(i).str() + "'"};
   //    evaluetedArguments.push_back(::move(result));
   // }

   // // call function
   // return function->execute(evaluetedArguments, environment);
}
//---------------------------------------------------------------------------
void FunctionOperator::print(ostream& stream) const
{
   stream << functionName << " id:" << functionIdentifier << endl;
}
//---------------------------------------------------------------------------
} // end of namespace harriet
//---------------------------------------------------------------------------
