#include "Expression.hpp"
#include "Environment.hpp"
#include "Function.hpp"
#include "Utility.hpp"
#include "Value.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <list>
#include <sstream>
#include <stack>
#include <vector>
//---------------------------------------------------------------------------
// Harriet Script Language
// Copyright (c) 2013 Alexander van Renen (alexandervanrenen@gmail.com)
// See the file LICENSE.txt for copying permission.
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace harriet {
//---------------------------------------------------------------------------
Expression::Expression(ExpressionType type)
: type(type)
, value(Value::createDefault(VariableType::createUndefinedType()))
{
}
//---------------------------------------------------------------------------
unique_ptr<Expression> Expression::createUndefinedExpression()
{
   return unique_ptr<Expression>(new Expression(ExpressionType::TUndefined));
}
//---------------------------------------------------------------------------
unique_ptr<Expression> Expression::createVariableExpression(const string& identifier)
{
   auto result = unique_ptr<Expression>(new Expression(ExpressionType::TVariable));
   result->identifier = identifier;
   return result;
}
//---------------------------------------------------------------------------
unique_ptr<Expression> Expression::createValueExpression(Value&& value)
{
   auto result = unique_ptr<Expression>(new Expression(ExpressionType::TValue));
   result->value = move(value);
   return result;
}
//---------------------------------------------------------------------------
unique_ptr<Expression> Expression::createValueReferenceExpression(Value* value)
{
   auto result = unique_ptr<Expression>(new Expression(ExpressionType::TValueReference));
   result->data.valueReference= value;
   return result;
}
//---------------------------------------------------------------------------
unique_ptr<Expression> Expression::createBinaryExpression(ExpressionType type, unique_ptr<Expression> lhs, unique_ptr<Expression> rhs)
{
   auto result = unique_ptr<Expression>(new Expression(type));
   result->data.lhs = lhs.release();
   result->data.rhs = rhs.release();
   return result;
}
//---------------------------------------------------------------------------
Expression::Expression(Expression&& other)
: type(other.type)
, value(Value::createDefault(VariableType::createUndefinedType()))
{
   other.type = ExpressionType::TUndefined;

   switch(type) {
      case ExpressionType::TUndefined:
         throw;
      case ExpressionType::TVariable:
         identifier = move(other.identifier);
         return;
      case ExpressionType::TValue:
         value = move(other.value);
         return;
      case ExpressionType::TValueReference:
         data.valueReference = other.data.valueReference;
         other.data.valueReference = nullptr;
         return;
      case ExpressionType::TPlusOperator:
      case ExpressionType::TMinusOperator:
      case ExpressionType::TMultiplicationOperator:
      case ExpressionType::TDivisionOperator:
      case ExpressionType::TAndOperator:
      case ExpressionType::TOrOperator:
      case ExpressionType::TGreaterThanOperator:
      case ExpressionType::TLessThanOperator:
      case ExpressionType::TGreaterEqualOperator:
      case ExpressionType::TLessEqualOperator:
      case ExpressionType::TEqualOperator:
      case ExpressionType::TNotEqualOperator:
         data.lhs = other.data.lhs;
         other.data.lhs = nullptr;
         data.rhs = other.data.rhs;
         other.data.rhs = nullptr;
         return;
   }
   throw "unreachable";
}
//---------------------------------------------------------------------------
unique_ptr<Expression> Expression::createCopy() const
{
   switch(type) {
      case ExpressionType::TUndefined:
         throw;
      case ExpressionType::TVariable:
         return createVariableExpression(identifier);
      case ExpressionType::TValue:
         return createValueExpression(value.createCopy());
      case ExpressionType::TValueReference:
         return createValueReferenceExpression(data.valueReference);
      case ExpressionType::TPlusOperator:
      case ExpressionType::TMinusOperator:
      case ExpressionType::TMultiplicationOperator:
      case ExpressionType::TDivisionOperator:
      case ExpressionType::TAndOperator:
      case ExpressionType::TOrOperator:
      case ExpressionType::TGreaterThanOperator:
      case ExpressionType::TLessThanOperator:
      case ExpressionType::TGreaterEqualOperator:
      case ExpressionType::TLessEqualOperator:
      case ExpressionType::TEqualOperator:
      case ExpressionType::TNotEqualOperator:
         return createBinaryExpression(type, data.lhs->createCopy(), data.rhs->createCopy());
   }
   throw "unreachable";
}
//---------------------------------------------------------------------------
Expression& Expression::operator= (Expression&& other)
{
   type = other.type;
   other.type = ExpressionType::TUndefined;

   switch(type) {
      case ExpressionType::TUndefined:
         throw;
      case ExpressionType::TVariable:
         identifier = move(other.identifier);
         return *this;
      case ExpressionType::TValue:
         value = move(other.value);
         return *this;
      case ExpressionType::TValueReference:
         data.valueReference = other.data.valueReference;
         other.data.valueReference = nullptr;
         return *this;
      case ExpressionType::TPlusOperator:
      case ExpressionType::TMinusOperator:
      case ExpressionType::TMultiplicationOperator:
      case ExpressionType::TDivisionOperator:
      case ExpressionType::TAndOperator:
      case ExpressionType::TOrOperator:
      case ExpressionType::TGreaterThanOperator:
      case ExpressionType::TLessThanOperator:
      case ExpressionType::TGreaterEqualOperator:
      case ExpressionType::TLessEqualOperator:
      case ExpressionType::TEqualOperator:
      case ExpressionType::TNotEqualOperator:
         data.lhs = other.data.lhs;
         other.data.lhs = nullptr;
         data.rhs = other.data.rhs;
         other.data.rhs = nullptr;
         return *this;
   }
   throw "unreachable";
}
//---------------------------------------------------------------------------
Expression::~Expression()
{
   switch(type) {
      case ExpressionType::TPlusOperator:
      case ExpressionType::TMinusOperator:
      case ExpressionType::TMultiplicationOperator:
      case ExpressionType::TDivisionOperator:
      case ExpressionType::TAndOperator:
      case ExpressionType::TOrOperator:
      case ExpressionType::TGreaterThanOperator:
      case ExpressionType::TLessThanOperator:
      case ExpressionType::TGreaterEqualOperator:
      case ExpressionType::TLessEqualOperator:
      case ExpressionType::TEqualOperator:
      case ExpressionType::TNotEqualOperator:
         delete data.lhs;
         delete data.rhs;
         return;
      default:
         return;
   }
   throw "unreachable";
}
//---------------------------------------------------------------------------
void Expression::print(ostream& stream) const
{
   switch(type) {
      case ExpressionType::TUndefined:
         throw;
      case ExpressionType::TVariable:
         stream << identifier; return;
      case ExpressionType::TValue:
         stream << value; return;
      case ExpressionType::TValueReference:
         stream << *data.valueReference; return;
      case ExpressionType::TPlusOperator:
         data.lhs->print(stream); stream << " + "; data.rhs->print(stream); return;
      case ExpressionType::TMinusOperator:
         data.lhs->print(stream); stream << " - "; data.rhs->print(stream); return;
      case ExpressionType::TMultiplicationOperator:
         data.lhs->print(stream); stream << " * "; data.rhs->print(stream); return;
      case ExpressionType::TDivisionOperator:
         data.lhs->print(stream); stream << " / "; data.rhs->print(stream); return;
      case ExpressionType::TAndOperator:
         data.lhs->print(stream); stream << " & "; data.rhs->print(stream); return;
      case ExpressionType::TOrOperator:
         data.lhs->print(stream); stream << " | "; data.rhs->print(stream); return;
      case ExpressionType::TGreaterThanOperator:
         data.lhs->print(stream); stream << " > "; data.rhs->print(stream); return;
      case ExpressionType::TLessThanOperator:
         data.lhs->print(stream); stream << " < "; data.rhs->print(stream); return;
      case ExpressionType::TGreaterEqualOperator:
         data.lhs->print(stream); stream << " >= "; data.rhs->print(stream); return;
      case ExpressionType::TLessEqualOperator:
         data.lhs->print(stream); stream << " <= "; data.rhs->print(stream); return;
      case ExpressionType::TEqualOperator:
         data.lhs->print(stream); stream << " == "; data.rhs->print(stream); return;
      case ExpressionType::TNotEqualOperator:
         data.lhs->print(stream); stream << " != "; data.rhs->print(stream); return;
   }
   throw "unreachable";
}
//---------------------------------------------------------------------------
Value Expression::evaluate(const Environment& environment) const
{
   switch(type) {
      case ExpressionType::TUndefined:
         throw;
      case ExpressionType::TVariable:
         return environment.read(identifier).createCopy();
      case ExpressionType::TValue:
         return value.createCopy();
      case ExpressionType::TValueReference:
         return data.valueReference->createCopy();
      case ExpressionType::TPlusOperator:
         return data.lhs->evaluate(environment).computeAdd(data.rhs->evaluate(environment));
      case ExpressionType::TMinusOperator:
         return data.lhs->evaluate(environment).computeSub(data.rhs->evaluate(environment));
      case ExpressionType::TMultiplicationOperator:
         return data.lhs->evaluate(environment).computeMul(data.rhs->evaluate(environment));
      case ExpressionType::TDivisionOperator:
         return data.lhs->evaluate(environment).computeDiv(data.rhs->evaluate(environment));
      case ExpressionType::TAndOperator:
         return data.lhs->evaluate(environment).computeAnd(data.rhs->evaluate(environment));
      case ExpressionType::TOrOperator:
         throw;
      case ExpressionType::TGreaterThanOperator:
         return data.lhs->evaluate(environment).computeGt(data.rhs->evaluate(environment));
      case ExpressionType::TLessThanOperator:
         return data.lhs->evaluate(environment).computeLt(data.rhs->evaluate(environment));
      case ExpressionType::TGreaterEqualOperator:
         return data.lhs->evaluate(environment).computeGeq(data.rhs->evaluate(environment));
      case ExpressionType::TLessEqualOperator:
         return data.lhs->evaluate(environment).computeLeq(data.rhs->evaluate(environment));
      case ExpressionType::TEqualOperator:
         if(data.lhs->type == ExpressionType::TValueReference && data.rhs->type == ExpressionType::TValue) {
            return data.lhs->data.valueReference->computeEq(data.rhs->value);
         } else if(data.lhs->type == ExpressionType::TValueReference && data.rhs->type == ExpressionType::TValueReference) {
            return data.lhs->data.valueReference->computeEq(*data.rhs->data.valueReference);
         } else {
            return data.lhs->evaluate(environment).computeEq(data.rhs->evaluate(environment));
         }
      case ExpressionType::TNotEqualOperator:
         throw;
   }
   throw "unreachable";
}
//---------------------------------------------------------------------------
ExpressionType Expression::getExpressionType() const
{
   return type;
}
//---------------------------------------------------------------------------
vector<string*> Expression::getAllVariableNames()
{
   vector<string*> result;
   switch(type) {
      case ExpressionType::TUndefined:
         throw;
      case ExpressionType::TVariable:
         result.push_back(&identifier); return result;
      case ExpressionType::TValue:
      case ExpressionType::TValueReference:
            return result;
      case ExpressionType::TPlusOperator:
      case ExpressionType::TMinusOperator:
      case ExpressionType::TMultiplicationOperator:
      case ExpressionType::TDivisionOperator:
      case ExpressionType::TAndOperator:
      case ExpressionType::TOrOperator:
      case ExpressionType::TGreaterThanOperator:
      case ExpressionType::TLessThanOperator:
      case ExpressionType::TGreaterEqualOperator:
      case ExpressionType::TLessEqualOperator:
      case ExpressionType::TEqualOperator:
      case ExpressionType::TNotEqualOperator:
         auto lhsResult = data.lhs->getAllVariableNames();
         for(auto iter : lhsResult)
            result.push_back(iter);
         auto rhsResult = data.rhs->getAllVariableNames();
         for(auto iter : rhsResult)
            result.push_back(iter);
         return result;
   }
   throw "unreachable";
}
//---------------------------------------------------------------------------
vector<Expression**> Expression::getAllVariables(Expression** self)
{
   vector<Expression**> result;
   switch(type) {
      case ExpressionType::TUndefined:
         throw;
      case ExpressionType::TVariable:
         result.push_back(self); return result;
      case ExpressionType::TValue:
      case ExpressionType::TValueReference:
            return result;
      case ExpressionType::TPlusOperator:
      case ExpressionType::TMinusOperator:
      case ExpressionType::TMultiplicationOperator:
      case ExpressionType::TDivisionOperator:
      case ExpressionType::TAndOperator:
      case ExpressionType::TOrOperator:
      case ExpressionType::TGreaterThanOperator:
      case ExpressionType::TLessThanOperator:
      case ExpressionType::TGreaterEqualOperator:
      case ExpressionType::TLessEqualOperator:
      case ExpressionType::TEqualOperator:
      case ExpressionType::TNotEqualOperator:
         auto lhsResult = data.lhs->getAllVariables(&data.lhs);
         for(auto iter : lhsResult)
            result.push_back(iter);
         auto rhsResult = data.rhs->getAllVariables(&data.rhs);
         for(auto iter : rhsResult)
            result.push_back(iter);
         return result;
   }
   throw "unreachable";
}
//---------------------------------------------------------------------------
bool Expression::isLogicOperator() const
{
   ExpressionType t = getExpressionType();
   return t==ExpressionType::TAndOperator || t==ExpressionType::TOrOperator || t==ExpressionType::TGreaterThanOperator || t==ExpressionType::TLessThanOperator || t==ExpressionType::TGreaterEqualOperator || t==ExpressionType::TLessEqualOperator || t==ExpressionType::TEqualOperator || t==ExpressionType::TNotEqualOperator;
}
//---------------------------------------------------------------------------
} // end of namespace harriet
//---------------------------------------------------------------------------
