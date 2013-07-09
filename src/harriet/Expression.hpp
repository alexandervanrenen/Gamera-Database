#ifndef SCRIPTLANGUAGE_EXPRESSION_HPP_
#define SCRIPTLANGUAGE_EXPRESSION_HPP_
//---------------------------------------------------------------------------
#include "GenericAllocator.hpp"
#include "ScriptLanguage.hpp"
#include "Value.hpp"
#include <cstring>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>
//---------------------------------------------------------------------------
// Harriet Script Language
// Copyright (c) 2012, 2013 Alexander van Renen (alexandervanrenen@gmail.com)
// See the file LICENSE.txt for copying permission.
//---------------------------------------------------------------------------
namespace harriet {
//---------------------------------------------------------------------------
class Environment;
class Value;
//---------------------------------------------------------------------------
enum struct ExpressionType : uint8_t {TUndefined, TVariable, TValue, TValueReference, TPlusOperator, TMinusOperator, TMultiplicationOperator, TDivisionOperator, TAndOperator, TOrOperator, TGreaterThanOperator, TLessThanOperator, TGreaterEqualOperator, TLessEqualOperator, TEqualOperator, TNotEqualOperator};
//---------------------------------------------------------------------------
class Expression {
   Expression(ExpressionType type);

public:
   /// Type of this expression
   ExpressionType type;

   /// The actual data of this expression
   std::string identifier;
   Value value; // TODO: merge them into the union
   union {
      Value* valueReference;
      Expression* child; // Unary operator
      struct { Expression* lhs; Expression* rhs; }; // Binary operator
   } data;

   static std::unique_ptr<Expression> createUndefinedExpression();
   static std::unique_ptr<Expression> createVariableExpression(const std::string& identifier);
   static std::unique_ptr<Expression> createValueExpression(Value&& value);
   static std::unique_ptr<Expression> createValueReferenceExpression(Value* value);
   static std::unique_ptr<Expression> createBinaryExpression(ExpressionType type, std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs);
   std::unique_ptr<Expression> createCopy() const;
   Expression(Expression&& other);
   Expression& operator= (Expression&& other);

   ~Expression();

   void print(std::ostream& stream) const;

   Value evaluate(const Environment& environment) const;
   std::vector<std::string*> getAllVariableNames();
   std::vector<Expression**> getAllVariables(Expression** self);

   ExpressionType getExpressionType() const;

   /// Type Categories
   bool isLogicOperator() const;
};
//---------------------------------------------------------------------------
} // end of namespace harriet
//---------------------------------------------------------------------------
#endif
