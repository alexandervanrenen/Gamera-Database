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
class Variable;
//---------------------------------------------------------------------------
enum struct ExpressionType : uint8_t {TVariable, TValue, TUnaryMinusOperator, TNotOperator, TIntegerCast, TFloatCast, TBoolCast, TAssignmentOperator, TPlusOperator, TMinusOperator, TMultiplicationOperator, TDivisionOperator, TModuloOperator, TExponentiationOperator, TAndOperator, TOrOperator, TGreaterOperator, TLessOperator, TGreaterEqualOperator, TLessEqualOperator, TEqualOperator, TNotEqualOperator, TFunctionOperator};
//---------------------------------------------------------------------------
class Expression {
public:
   virtual void print(std::ostream& stream) const = 0;

   virtual Value evaluate(Environment& environment) const = 0;
   virtual std::vector<std::unique_ptr<Expression>*> getAllVariables(std::unique_ptr<Expression>* self) = 0; // Pointer to the place where I am stored

   virtual ExpressionType getExpressionType() const = 0;

   virtual ~Expression(){};

   /// Type Categories
   bool isLogicOperator() const;
};
//---------------------------------------------------------------------------
class Variable : public Expression {
public:
   Variable(const std::string& identifier) : identifier(identifier) {}
   virtual ~Variable(){};
   virtual void print(std::ostream& stream) const;
   virtual Value evaluate(Environment& environment) const;
   virtual std::vector<std::unique_ptr<Expression>*> getAllVariables(std::unique_ptr<Expression>* self) {return std::vector<std::unique_ptr<Expression>*>(1, self);}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TVariable;}
   const std::string& getIdentifier() const {return identifier;}

protected:
   std::string identifier;
};
//---------------------------------------------------------------------------
class ValueExpression : public Expression {
public:
   ValueExpression(Value&& value) : value(std::move(value)) {}
   virtual ~ValueExpression(){};
   virtual void print(std::ostream& stream) const;
   virtual Value evaluate(Environment& environment) const;
   virtual std::vector<std::unique_ptr<Expression>*> getAllVariables(std::unique_ptr<Expression>*) {return std::vector<std::unique_ptr<Expression>*>();}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TValue;}
   Value value;
};
//---------------------------------------------------------------------------
class UnaryOperator : public Expression {
   virtual std::vector<std::unique_ptr<Expression>*> getAllVariables(std::unique_ptr<Expression>* self);
   virtual void print(std::ostream& stream) const;
public:
   virtual void addChild(std::unique_ptr<Expression> child);
   virtual ~UnaryOperator(){};
protected:
   std::unique_ptr<Expression> child;
   virtual const std::string getSign() const = 0;
   friend class ExpressionParser;
};
//---------------------------------------------------------------------------
class UnaryMinusOperator : public UnaryOperator {
   virtual Value evaluate(Environment& environment) const;
   virtual ExpressionType getExpressionType() const {return ExpressionType::TUnaryMinusOperator;}
protected:
   virtual const std::string getSign() const {return "-";}
};
//---------------------------------------------------------------------------
class NotOperator : public UnaryOperator {
   virtual Value evaluate(Environment& environment) const;
   virtual ExpressionType getExpressionType() const {return ExpressionType::TNotOperator;}
protected:
   virtual const std::string getSign() const {return "!";}
};
//---------------------------------------------------------------------------
class BinaryOperator : public Expression {
public:
   virtual ~BinaryOperator(){}
   virtual std::vector<std::unique_ptr<Expression>*> getAllVariables(std::unique_ptr<Expression>* self);
   std::unique_ptr<Expression> lhs;
   std::unique_ptr<Expression> rhs;
protected:
   virtual void print(std::ostream& stream) const;
   virtual void addChildren(std::unique_ptr<Expression> lhsChild, std::unique_ptr<Expression> rhsChild);
   virtual const std::string getSign() const = 0;
   friend class ExpressionParser;
};
//---------------------------------------------------------------------------
class AssignmentOperator : public BinaryOperator {
public:
   virtual ~AssignmentOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TAssignmentOperator;}
protected:
   virtual Value evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "=";}
};
//---------------------------------------------------------------------------
class ArithmeticOperator : public BinaryOperator {
};
//---------------------------------------------------------------------------
class PlusOperator : public ArithmeticOperator {
public:
   PlusOperator(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs) {this->lhs=move(lhs); this->rhs=move(rhs);}
   virtual ~PlusOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TPlusOperator;}
protected:
   virtual Value evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "+";}
};
//---------------------------------------------------------------------------
class MinusOperator : public ArithmeticOperator {
public:
   MinusOperator(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs) {this->lhs=move(lhs); this->rhs=move(rhs);}
   virtual ~MinusOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TMinusOperator;}
protected:
   virtual Value evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "-";}
};
//---------------------------------------------------------------------------
class MultiplicationOperator : public ArithmeticOperator {
public:
   MultiplicationOperator(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs) {this->lhs=move(lhs); this->rhs=move(rhs);}
   virtual ~MultiplicationOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TMultiplicationOperator;}
protected:
   virtual Value evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "*";}
};
//---------------------------------------------------------------------------
class DivisionOperator : public ArithmeticOperator {
public:
   DivisionOperator(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs) {this->lhs=move(lhs); this->rhs=move(rhs);}
   virtual ~DivisionOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TDivisionOperator;}
protected:
   virtual Value evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "/";}
};
//---------------------------------------------------------------------------
class ModuloOperator : public ArithmeticOperator {
public:
   virtual ~ModuloOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TModuloOperator;}
protected:
   virtual Value evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "%";}
};
//---------------------------------------------------------------------------
class ExponentiationOperator : public ArithmeticOperator {
public:
   virtual ~ExponentiationOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TExponentiationOperator;}
protected:
   virtual Value evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "^";}
};
//---------------------------------------------------------------------------
class LogicOperator : public BinaryOperator {
};
//---------------------------------------------------------------------------
class AndOperator : public LogicOperator {
public:
   AndOperator(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs);
   virtual ~AndOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TAndOperator;}
protected:
   virtual Value evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "&";}
};
//---------------------------------------------------------------------------
class OrOperator : public LogicOperator {
public:
   virtual ~OrOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TOrOperator;}
protected:
   virtual Value evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "|";}
};
//---------------------------------------------------------------------------
class ComparisonOperator : public BinaryOperator {
public:
   virtual ~ComparisonOperator(){}
};
//---------------------------------------------------------------------------
class GreaterOperator : public ComparisonOperator {
public:
   virtual ~GreaterOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TGreaterOperator;}
protected:
   virtual Value evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return ">";}
};
//---------------------------------------------------------------------------
class LessOperator : public ComparisonOperator {
public:
   virtual ~LessOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TLessOperator;}
protected:
   virtual Value evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "<";}
};
//---------------------------------------------------------------------------
class GreaterEqualOperator : public ComparisonOperator {
public:
   virtual ~GreaterEqualOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TGreaterEqualOperator;}
protected:
   virtual Value evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return ">=";}
};
//---------------------------------------------------------------------------
class LessEqualOperator : public ComparisonOperator {
public:
   virtual ~LessEqualOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TLessEqualOperator;}
protected:
   virtual Value evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "<=";}
};
//---------------------------------------------------------------------------
class EqualOperator : public ComparisonOperator {
public:
   EqualOperator(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs) {this->lhs=move(lhs); this->rhs=move(rhs);}
   virtual ~EqualOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TEqualOperator;}
protected:
   virtual Value evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "==";}
};
//---------------------------------------------------------------------------
class NotEqualOperator : public ComparisonOperator {
public:
   virtual ~NotEqualOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TNotEqualOperator;}
protected:
   virtual Value evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "!=";}
};
//---------------------------------------------------------------------------
class FunctionOperator : public Expression { // AAA inherit from value ?
   virtual void print(std::ostream& stream) const;
public:
   FunctionOperator(const std::string& functionName, uint32_t functionIdentifier, std::vector<std::unique_ptr<Expression>>& arguments);
   virtual ~FunctionOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TFunctionOperator;}
protected:
   virtual Value evaluate(Environment& environment) const;

   const std::string functionName;
   const uint32_t functionIdentifier;
   const std::vector<std::unique_ptr<Expression>> arguments;

   friend class ExpressionParser;
};
//---------------------------------------------------------------------------
} // end of namespace harriet
//---------------------------------------------------------------------------
#endif
