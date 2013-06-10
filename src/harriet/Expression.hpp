#ifndef SCRIPTLANGUAGE_EXPRESSION_HPP_
#define SCRIPTLANGUAGE_EXPRESSION_HPP_
//---------------------------------------------------------------------------
#include "ScriptLanguage.hpp"
#include "vector3.hpp"
#include "GenericAllocator.hpp"
#include <memory>
#include <string>
#include <iostream>
#include <stack>
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
enum struct ExpressionType : uint8_t {TVariable, TIntegerValue, TFloatValue, TBoolValue, TVectorValue, TUnaryMinusOperator, TNotOperator, TIntegerCast, TFloatCast, TBoolCast, TVectorCast, TAssignmentOperator, TPlusOperator, TMinusOperator, TMultiplicationOperator, TDivisionOperator, TModuloOperator, TExponentiationOperator, TAndOperator, TOrOperator, TGreaterOperator, TLessOperator, TGreaterEqualOperator, TLessEqualOperator, TEqualOperator, TNotEqualOperator, TFunctionOperator};
//---------------------------------------------------------------------------
class Expression {
public:
   virtual void print(std::ostream& stream) const = 0;

   virtual std::unique_ptr<Value> evaluate(Environment& environment) const = 0;
   virtual std::vector<const Variable*> getAllVariables() const = 0;

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
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
   virtual std::vector<const Variable*> getAllVariables() const {return {{this}};}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TVariable;}
   const std::string& getIdentifier() const {return identifier;}

protected:
   std::string identifier;
};
//---------------------------------------------------------------------------
class Value : public Expression {
public:
   virtual harriet::VariableType getResultType() const = 0;
   virtual uint32_t typeSize() const = 0;
   virtual std::unique_ptr<Value> evaluate(Environment& /*environment*/) const {return evaluate();}
   virtual std::unique_ptr<Value> evaluate() const = 0;
   virtual std::vector<const Variable*> getAllVariables() const {return std::vector<const Variable*>();};

   virtual std::unique_ptr<Value> computeAdd(const Value& rhs) const {doError("+" , *this, rhs); throw;}
   virtual std::unique_ptr<Value> computeSub(const Value& rhs) const {doError("-" , *this, rhs); throw;}
   virtual std::unique_ptr<Value> computeMul(const Value& rhs) const {doError("*" , *this, rhs); throw;}
   virtual std::unique_ptr<Value> computeDiv(const Value& rhs) const {doError("/" , *this, rhs); throw;}
   virtual std::unique_ptr<Value> computeMod(const Value& rhs) const {doError("%" , *this, rhs); throw;}
   virtual std::unique_ptr<Value> computeExp(const Value& rhs) const {doError("^" , *this, rhs); throw;}
   virtual std::unique_ptr<Value> computeAnd(const Value& rhs) const {doError("&" , *this, rhs); throw;}
   virtual std::unique_ptr<Value> computeOr (const Value& rhs) const {doError("|" , *this, rhs); throw;}
   virtual std::unique_ptr<Value> computeGt (const Value& rhs) const {doError(">" , *this, rhs); throw;}
   virtual std::unique_ptr<Value> computeLt (const Value& rhs) const {doError("<" , *this, rhs); throw;}
   virtual std::unique_ptr<Value> computeGeq(const Value& rhs) const {doError(">=", *this, rhs); throw;}
   virtual std::unique_ptr<Value> computeLeq(const Value& rhs) const {doError("<=", *this, rhs); throw;}
   virtual std::unique_ptr<Value> computeEq (const Value& rhs) const {doError("==", *this, rhs); throw;}
   virtual std::unique_ptr<Value> computeNeq(const Value& rhs) const {doError("!=", *this, rhs); throw;}

   virtual std::unique_ptr<Value> computeInv() const {doError("-", *this); throw;}
   virtual std::unique_ptr<Value> computeNot() const {doError("!", *this); throw;}

   virtual std::unique_ptr<Value> computeCast(harriet::VariableType resultType) const {throw harriet::Exception{"unable to cast '" + harriet::typeToName(getResultType()) + "' to '" +  harriet::typeToName(resultType) + "'"};} // TODO: remove environment parameter

   friend std::ostream& operator<< (std::ostream& os, const Value& value) {value.print(os); return os;}

protected:
   static void doError(const std::string& operatorSign, const Value& lhs, const Value& rhs) throw(harriet::Exception) { throw harriet::Exception{"binary operator '" + operatorSign + "' does not accept '" + harriet::typeToName(lhs.getResultType()) + "' and '" + harriet::typeToName(rhs.getResultType()) + "'"}; }
   static void doError(const std::string& operatorSign, const Value& lhs) throw(harriet::Exception) { throw harriet::Exception{"unary operator '" + operatorSign + "' does not accept '" + harriet::typeToName(lhs.getResultType()) + "'"}; }
};
//---------------------------------------------------------------------------
struct IntegerValue : public Value, GenericAllocator<IntegerValue> {
   using GenericAllocator<IntegerValue>::operator new;
   using GenericAllocator<IntegerValue>::operator delete;

   virtual void print(std::ostream& stream) const;
   virtual std::unique_ptr<Value> evaluate() const;
   int32_t result;
   IntegerValue(int32_t result) : result(result) {}
   virtual ~IntegerValue(){};
   virtual ExpressionType getExpressionType() const {return ExpressionType::TIntegerValue;}

   virtual harriet::VariableType getResultType() const {return harriet::VariableType::TInteger;}
   virtual uint32_t typeSize() const {return sizeof(int32_t);}

   virtual std::unique_ptr<Value> computeAdd(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeSub(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeMul(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeDiv(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeMod(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeExp(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeAnd(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeOr (const Value& rhs) const;
   virtual std::unique_ptr<Value> computeGt (const Value& rhs) const;
   virtual std::unique_ptr<Value> computeLt (const Value& rhs) const;
   virtual std::unique_ptr<Value> computeGeq(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeLeq(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeEq (const Value& rhs) const;
   virtual std::unique_ptr<Value> computeNeq(const Value& rhs) const;

   virtual std::unique_ptr<Value> computeInv() const;

   virtual std::unique_ptr<Value> computeCast(harriet::VariableType resultType) const;
};
//---------------------------------------------------------------------------
struct FloatValue : public Value, GenericAllocator<FloatValue> {
   using GenericAllocator<FloatValue>::operator new;
   using GenericAllocator<FloatValue>::operator delete;

   virtual void print(std::ostream& stream) const;
   virtual std::unique_ptr<Value> evaluate() const;
   float result;
   FloatValue(float result) : result(result) {}
   virtual ~FloatValue(){};
   virtual ExpressionType getExpressionType() const {return ExpressionType::TFloatValue;}

   virtual harriet::VariableType getResultType() const {return harriet::VariableType::TFloat;}
   virtual uint32_t typeSize() const {return sizeof(float);}

   virtual std::unique_ptr<Value> computeAdd(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeSub(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeMul(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeDiv(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeMod(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeExp(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeGt (const Value& rhs) const;
   virtual std::unique_ptr<Value> computeLt (const Value& rhs) const;
   virtual std::unique_ptr<Value> computeGeq(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeLeq(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeEq (const Value& rhs) const;
   virtual std::unique_ptr<Value> computeNeq(const Value& rhs) const;

   virtual std::unique_ptr<Value> computeInv() const;

   virtual std::unique_ptr<Value> computeCast(harriet::VariableType resultType) const;
};
//---------------------------------------------------------------------------
struct BoolValue : public Value, GenericAllocator<BoolValue> {
   using GenericAllocator<BoolValue>::operator new;
   using GenericAllocator<BoolValue>::operator delete;

   virtual void print(std::ostream& stream) const;
   virtual std::unique_ptr<Value> evaluate() const;
   bool result;
   BoolValue(bool result) : result(result) {}
   virtual ~BoolValue(){};
   virtual ExpressionType getExpressionType() const {return ExpressionType::TBoolValue;}

   virtual harriet::VariableType getResultType() const {return harriet::VariableType::TBool;}
   virtual uint32_t typeSize() const {return sizeof(bool);}

   virtual std::unique_ptr<Value> computeAnd(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeOr (const Value& rhs) const;
   virtual std::unique_ptr<Value> computeEq (const Value& rhs) const;
   virtual std::unique_ptr<Value> computeNeq(const Value& rhs) const;

   virtual std::unique_ptr<Value> computeNot() const;

   virtual std::unique_ptr<Value> computeCast(harriet::VariableType resultType) const;
};
//---------------------------------------------------------------------------
struct VectorValue : public Value, GenericAllocator<VectorValue> {
   using GenericAllocator<VectorValue>::operator new;
   using GenericAllocator<VectorValue>::operator delete;

   virtual void print(std::ostream& stream) const;
   virtual std::unique_ptr<Value> evaluate() const;
   Vector3<float> result;
   VectorValue(const Vector3<float>& result) : result(result) {}
   virtual ~VectorValue(){};
   virtual ExpressionType getExpressionType() const {return ExpressionType::TVectorValue;}

   virtual harriet::VariableType getResultType() const {return harriet::VariableType::TVector;}
   virtual uint32_t typeSize() const {return sizeof(Vector3<float>);}

   virtual std::unique_ptr<Value> computeAdd(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeSub(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeMul(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeDiv(const Value& rhs) const;
   virtual std::unique_ptr<Value> computeEq (const Value& rhs) const;
   virtual std::unique_ptr<Value> computeNeq(const Value& rhs) const;

   virtual std::unique_ptr<Value> computeInv() const;

   virtual std::unique_ptr<Value> computeCast(harriet::VariableType resultType) const;
};
//---------------------------------------------------------------------------
class UnaryOperator : public Expression {
   virtual std::vector<const Variable*> getAllVariables() const {return child->getAllVariables();}
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
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
   virtual ExpressionType getExpressionType() const {return ExpressionType::TUnaryMinusOperator;}
protected:
   virtual const std::string getSign() const {return "-";}
};
//---------------------------------------------------------------------------
class NotOperator : public UnaryOperator {
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
   virtual ExpressionType getExpressionType() const {return ExpressionType::TNotOperator;}
protected:
   virtual const std::string getSign() const {return "!";}
};
//---------------------------------------------------------------------------
class CastOperator : public UnaryOperator {
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const; // uses getCastType to determin the result type
protected:
   virtual harriet::VariableType getCastType() const = 0;
};
//---------------------------------------------------------------------------
class IntegerCast : public CastOperator {
   virtual ExpressionType getExpressionType() const {return ExpressionType::TIntegerCast;}
   virtual harriet::VariableType getCastType() const {return harriet::VariableType::TInteger;}
   virtual const std::string getSign() const {return "cast<int>";}
};
//---------------------------------------------------------------------------
class FloatCast : public CastOperator {
   virtual ExpressionType getExpressionType() const {return ExpressionType::TFloatCast;}
   virtual harriet::VariableType getCastType() const {return harriet::VariableType::TFloat;}
   virtual const std::string getSign() const {return "cast<float>";}
};
//---------------------------------------------------------------------------
class BoolCast : public CastOperator {
   virtual ExpressionType getExpressionType() const {return ExpressionType::TBoolCast;}
   virtual harriet::VariableType getCastType() const {return harriet::VariableType::TBool;}
   virtual const std::string getSign() const {return "cast<bool>";}
};
//---------------------------------------------------------------------------
class VectorCast : public CastOperator {
   virtual ExpressionType getExpressionType() const {return ExpressionType::TVectorCast;}
   virtual harriet::VariableType getCastType() const {return harriet::VariableType::TVector;}
   virtual const std::string getSign() const {return "cast<vector>";}
};
//---------------------------------------------------------------------------
class BinaryOperator : public Expression {
public:
   virtual ~BinaryOperator(){}
   virtual std::vector<const Variable*> getAllVariables() const {auto l=lhs->getAllVariables(); auto r=rhs->getAllVariables(); l.insert(l.end(), r.begin(), r.end()); return l;}
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
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
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
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "+";}
};
//---------------------------------------------------------------------------
class MinusOperator : public ArithmeticOperator {
public:
   MinusOperator(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs) {this->lhs=move(lhs); this->rhs=move(rhs);}
   virtual ~MinusOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TMinusOperator;}
protected:
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "-";}
};
//---------------------------------------------------------------------------
class MultiplicationOperator : public ArithmeticOperator {
public:
   MultiplicationOperator(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs) {this->lhs=move(lhs); this->rhs=move(rhs);}
   virtual ~MultiplicationOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TMultiplicationOperator;}
protected:
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "*";}
};
//---------------------------------------------------------------------------
class DivisionOperator : public ArithmeticOperator {
public:
   DivisionOperator(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs) {this->lhs=move(lhs); this->rhs=move(rhs);}
   virtual ~DivisionOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TDivisionOperator;}
protected:
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "/";}
};
//---------------------------------------------------------------------------
class ModuloOperator : public ArithmeticOperator {
public:
   virtual ~ModuloOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TModuloOperator;}
protected:
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "%";}
};
//---------------------------------------------------------------------------
class ExponentiationOperator : public ArithmeticOperator {
public:
   virtual ~ExponentiationOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TExponentiationOperator;}
protected:
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "^";}
};
//---------------------------------------------------------------------------
class LogicOperator : public BinaryOperator {
};
//---------------------------------------------------------------------------
class AndOperator : public LogicOperator {
public:
   virtual ~AndOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TAndOperator;}
protected:
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "&";}
};
//---------------------------------------------------------------------------
class OrOperator : public LogicOperator {
public:
   virtual ~OrOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TOrOperator;}
protected:
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
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
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return ">";}
};
//---------------------------------------------------------------------------
class LessOperator : public ComparisonOperator {
public:
   virtual ~LessOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TLessOperator;}
protected:
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "<";}
};
//---------------------------------------------------------------------------
class GreaterEqualOperator : public ComparisonOperator {
public:
   virtual ~GreaterEqualOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TGreaterEqualOperator;}
protected:
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return ">=";}
};
//---------------------------------------------------------------------------
class LessEqualOperator : public ComparisonOperator {
public:
   virtual ~LessEqualOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TLessEqualOperator;}
protected:
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "<=";}
};
//---------------------------------------------------------------------------
class EqualOperator : public ComparisonOperator {
public:
   EqualOperator(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs) {this->lhs=move(lhs); this->rhs=move(rhs);}
   virtual ~EqualOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TEqualOperator;}
protected:
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
   virtual const std::string getSign() const {return "==";}
};
//---------------------------------------------------------------------------
class NotEqualOperator : public ComparisonOperator {
public:
   virtual ~NotEqualOperator(){}
   virtual ExpressionType getExpressionType() const {return ExpressionType::TNotEqualOperator;}
protected:
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
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
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;

   const std::string functionName;
   const uint32_t functionIdentifier;
   const std::vector<std::unique_ptr<Expression>> arguments;

   friend class ExpressionParser;
};
//---------------------------------------------------------------------------
} // end of namespace harriet
//---------------------------------------------------------------------------
#endif
