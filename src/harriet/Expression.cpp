#include "Expression.hpp"
#include "Utility.hpp"
#include "Environment.hpp"
#include "Function.hpp"
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
   switch(result.getResultType()) {
      case harriet::VariableType::TInteger:    return make_unique<IntegerValue>(reinterpret_cast<const IntegerValue&>(result).result);
      case harriet::VariableType::TFloat:      return make_unique<FloatValue>(reinterpret_cast<const FloatValue&>(result).result);
      case harriet::VariableType::TBool:       return make_unique<BoolValue>(reinterpret_cast<const BoolValue&>(result).result);
      case harriet::VariableType::TVector:     return make_unique<VectorValue>(reinterpret_cast<const VectorValue&>(result).result);
      case harriet::VariableType::TCharacter:  return make_unique<CharacterValue>(reinterpret_cast<const CharacterValue&>(result).result, reinterpret_cast<const CharacterValue&>(result).result.size());
      default:                                     throw;
   }
}
//---------------------------------------------------------------------------
void IntegerValue::print(ostream& stream) const
{
   stream << result;
}
//---------------------------------------------------------------------------
unique_ptr<Value> IntegerValue::evaluate() const
{
   return make_unique<IntegerValue>(result);
}
//---------------------------------------------------------------------------
unique_ptr<Value> IntegerValue::computeAdd(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<IntegerValue>(this->result + reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<FloatValue>(this->result + reinterpret_cast<const FloatValue*>(&rhs)->result);
      case harriet::VariableType::TVector:  return make_unique<VectorValue>(Vector3<float>(reinterpret_cast<const FloatValue*>(&rhs)->result).add(this->result));
      default:                                     throw harriet::Exception{"invalid input for binary operator '+'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> IntegerValue::computeSub(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<IntegerValue>(this->result - reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<FloatValue>(this->result - reinterpret_cast<const FloatValue*>(&rhs)->result);
      case harriet::VariableType::TVector:  return make_unique<VectorValue>(Vector3<float>(reinterpret_cast<const FloatValue*>(&rhs)->result).sub(this->result));
      default:                                     throw harriet::Exception{"invalid input for binary operator '-'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> IntegerValue::computeMul(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<IntegerValue>(this->result * reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<FloatValue>(this->result * reinterpret_cast<const FloatValue*>(&rhs)->result);
      case harriet::VariableType::TVector:  return make_unique<VectorValue>(Vector3<float>(reinterpret_cast<const FloatValue*>(&rhs)->result).mul(this->result));
      default:                                     throw harriet::Exception{"invalid input for binary operator '*'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> IntegerValue::computeDiv(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<IntegerValue>(this->result / reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<FloatValue>(this->result / reinterpret_cast<const FloatValue*>(&rhs)->result);
      case harriet::VariableType::TVector:  return make_unique<VectorValue>(Vector3<float>(reinterpret_cast<const FloatValue*>(&rhs)->result).div(this->result));
      default:                                     throw harriet::Exception{"invalid input for binary operator '/'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> IntegerValue::computeMod(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: if(reinterpret_cast<const IntegerValue*>(&rhs)->result==0) return make_unique<IntegerValue>(0); else return make_unique<IntegerValue>(this->result % reinterpret_cast<const IntegerValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '%'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> IntegerValue::computeExp(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<IntegerValue>(static_cast<int32_t>(pow(this->result, reinterpret_cast<const IntegerValue*>(&rhs)->result)));
      case harriet::VariableType::TFloat:   return make_unique<IntegerValue>(static_cast<float>(pow(this->result, reinterpret_cast<const FloatValue*>(&rhs)->result)));
      default:                                     throw harriet::Exception{"invalid input for binary operator '^'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> IntegerValue::computeAnd(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<IntegerValue>(this->result & reinterpret_cast<const IntegerValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '&'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> IntegerValue::computeOr (const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<IntegerValue>(this->result | reinterpret_cast<const IntegerValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '|'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> IntegerValue::computeGt (const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<BoolValue>(this->result > reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<BoolValue>(this->result > reinterpret_cast<const FloatValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '>'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> IntegerValue::computeLt (const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<BoolValue>(this->result < reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<BoolValue>(this->result < reinterpret_cast<const FloatValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '<'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> IntegerValue::computeGeq(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<BoolValue>(this->result >= reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<BoolValue>(this->result >= reinterpret_cast<const FloatValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '>='"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> IntegerValue::computeLeq(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<BoolValue>(this->result >= reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<BoolValue>(this->result <= reinterpret_cast<const FloatValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '<='"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> IntegerValue::computeEq (const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<BoolValue>(this->result == reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<BoolValue>(this->result == reinterpret_cast<const FloatValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '=='"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> IntegerValue::computeNeq(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<BoolValue>(this->result != reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<BoolValue>(this->result != reinterpret_cast<const FloatValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '!='"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> IntegerValue::computeInv() const
{
                                                   return make_unique<IntegerValue>(-this->result);
}
//---------------------------------------------------------------------------
unique_ptr<Value> IntegerValue::computeCast(harriet::VariableType resultType) const
{
   switch(resultType) {
      case harriet::VariableType::TInteger: return make_unique<IntegerValue>(this->result);
      case harriet::VariableType::TFloat:   return make_unique<FloatValue>(this->result);
      case harriet::VariableType::TBool:    return make_unique<BoolValue>(this->result!=0);
      case harriet::VariableType::TVector:  return make_unique<VectorValue>(Vector3<float>(this->result, this->result, this->result));
      default:                                     throw harriet::Exception{"invalid cast target: '" + harriet::typeToName(resultType) + "'"};
   }
}
//---------------------------------------------------------------------------
void FloatValue::print(ostream& stream) const
{
   stream << result;
}
//---------------------------------------------------------------------------
unique_ptr<Value> FloatValue::evaluate() const
{
   return make_unique<FloatValue>(result);
}
//---------------------------------------------------------------------------
unique_ptr<Value> FloatValue::computeAdd(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<FloatValue>(this->result + reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<FloatValue>(this->result + reinterpret_cast<const FloatValue*>(&rhs)->result);
      case harriet::VariableType::TVector:  return make_unique<VectorValue>(Vector3<float>(reinterpret_cast<const FloatValue*>(&rhs)->result).add(this->result));
      default:                                     throw harriet::Exception{"invalid input for binary operator '+'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> FloatValue::computeSub(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<FloatValue>(this->result - reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<FloatValue>(this->result - reinterpret_cast<const FloatValue*>(&rhs)->result);
      case harriet::VariableType::TVector:  return make_unique<VectorValue>(Vector3<float>(reinterpret_cast<const FloatValue*>(&rhs)->result).sub(this->result));
      default:                                     throw harriet::Exception{"invalid input for binary operator '-'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> FloatValue::computeMul(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<FloatValue>(this->result * reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<FloatValue>(this->result * reinterpret_cast<const FloatValue*>(&rhs)->result);
      case harriet::VariableType::TVector:  return make_unique<VectorValue>(Vector3<float>(reinterpret_cast<const FloatValue*>(&rhs)->result).mul(this->result));
      default:                                     throw harriet::Exception{"invalid input for binary operator '*'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> FloatValue::computeDiv(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<FloatValue>(this->result / reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<FloatValue>(this->result / reinterpret_cast<const FloatValue*>(&rhs)->result);
      case harriet::VariableType::TVector:  return make_unique<VectorValue>(Vector3<float>(reinterpret_cast<const FloatValue*>(&rhs)->result).div(this->result));
      default:                                     throw harriet::Exception{"invalid input for binary operator '/'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> FloatValue::computeMod(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<FloatValue>(static_cast<int32_t>(this->result) % reinterpret_cast<const IntegerValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '%'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> FloatValue::computeExp(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<IntegerValue>(static_cast<float>(pow(this->result, reinterpret_cast<const IntegerValue*>(&rhs)->result)));
      case harriet::VariableType::TFloat:   return make_unique<IntegerValue>(static_cast<float>(pow(this->result, reinterpret_cast<const FloatValue*>(&rhs)->result)));
      default:                                     throw harriet::Exception{"invalid input for binary operator '^'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> FloatValue::computeGt (const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<BoolValue>(this->result > reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<BoolValue>(this->result > reinterpret_cast<const FloatValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '>'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> FloatValue::computeLt (const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<BoolValue>(this->result < reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<BoolValue>(this->result < reinterpret_cast<const FloatValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '<'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> FloatValue::computeGeq(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<BoolValue>(this->result >= reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<BoolValue>(this->result >= reinterpret_cast<const FloatValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '>='"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> FloatValue::computeLeq(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<BoolValue>(this->result >= reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<BoolValue>(this->result <= reinterpret_cast<const FloatValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '<='"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> FloatValue::computeEq (const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<BoolValue>(this->result == reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<BoolValue>(this->result == reinterpret_cast<const FloatValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '=='"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> FloatValue::computeNeq(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<BoolValue>(this->result != reinterpret_cast<const IntegerValue*>(&rhs)->result);
      case harriet::VariableType::TFloat:   return make_unique<BoolValue>(this->result != reinterpret_cast<const FloatValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '!='"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> FloatValue::computeInv() const
{
                                                   return make_unique<FloatValue>(-this->result);
}
//---------------------------------------------------------------------------
unique_ptr<Value> FloatValue::computeCast(harriet::VariableType resultType) const
{
   switch(resultType) {
      case harriet::VariableType::TInteger: return make_unique<IntegerValue>(this->result);
      case harriet::VariableType::TFloat:   return make_unique<FloatValue>(this->result);
      case harriet::VariableType::TBool:    return make_unique<BoolValue>(this->result!=0);
      case harriet::VariableType::TVector:  return make_unique<VectorValue>(Vector3<float>(this->result, this->result, this->result));
      default: throw harriet::Exception{"invalid cast target: '" + harriet::typeToName(resultType) + "'"};
   }
}
//---------------------------------------------------------------------------
void BoolValue::print(ostream& stream) const
{
   stream << (result?harriet::kTrue:harriet::kFalse);
}
//---------------------------------------------------------------------------
unique_ptr<Value> BoolValue::evaluate() const
{
   return make_unique<BoolValue>(result);
}
//---------------------------------------------------------------------------
unique_ptr<Value> BoolValue::computeAnd(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TBool:    return make_unique<BoolValue>(this->result & reinterpret_cast<const BoolValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '&'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> BoolValue::computeOr (const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TBool:    return make_unique<BoolValue>(this->result | reinterpret_cast<const BoolValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '|'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> BoolValue::computeEq (const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TBool:    return make_unique<BoolValue>(this->result == reinterpret_cast<const BoolValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '=='"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> BoolValue::computeNeq(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TBool:    return make_unique<BoolValue>(this->result != reinterpret_cast<const BoolValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '!='"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> BoolValue::computeNot() const
{
                                                   return make_unique<BoolValue>(!this->result);
}
//---------------------------------------------------------------------------
unique_ptr<Value> BoolValue::computeCast(harriet::VariableType resultType) const
{
   switch(resultType) {
      case harriet::VariableType::TInteger: return make_unique<IntegerValue>(this->result);
      case harriet::VariableType::TFloat:   return make_unique<FloatValue>(this->result);
      case harriet::VariableType::TBool:    return make_unique<BoolValue>(this->result);
      case harriet::VariableType::TVector:  return make_unique<VectorValue>(Vector3<float>(this->result, this->result, this->result));
      default:                                     throw harriet::Exception{"invalid cast target: '" + harriet::typeToName(resultType) + "'"};
   }
}
//---------------------------------------------------------------------------
void VectorValue::print(ostream& stream) const
{
   stream << result;
}
//---------------------------------------------------------------------------
unique_ptr<Value> VectorValue::evaluate() const
{
   return make_unique<VectorValue>(result);
}
//---------------------------------------------------------------------------
unique_ptr<Value> VectorValue::computeAdd(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<VectorValue>(Vector3<float>(this->result).add(reinterpret_cast<const IntegerValue*>(&rhs)->result));
      case harriet::VariableType::TFloat:   return make_unique<VectorValue>(Vector3<float>(this->result).add(reinterpret_cast<const FloatValue*>(&rhs)->result));
      case harriet::VariableType::TVector:  return make_unique<VectorValue>(this->result + reinterpret_cast<const VectorValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '+'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> VectorValue::computeSub(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<VectorValue>(Vector3<float>(this->result).sub(reinterpret_cast<const IntegerValue*>(&rhs)->result));
      case harriet::VariableType::TFloat:   return make_unique<VectorValue>(Vector3<float>(this->result).sub(reinterpret_cast<const FloatValue*>(&rhs)->result));
      case harriet::VariableType::TVector:  return make_unique<VectorValue>(this->result - reinterpret_cast<const VectorValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '-'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> VectorValue::computeMul(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<VectorValue>(Vector3<float>(this->result).mul(reinterpret_cast<const IntegerValue*>(&rhs)->result));
      case harriet::VariableType::TFloat:   return make_unique<VectorValue>(Vector3<float>(this->result).mul(reinterpret_cast<const FloatValue*>(&rhs)->result));
      default:                                     throw harriet::Exception{"invalid input for binary operator '*'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> VectorValue::computeDiv(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TInteger: return make_unique<VectorValue>(Vector3<float>(this->result).div(reinterpret_cast<const IntegerValue*>(&rhs)->result));
      case harriet::VariableType::TFloat:   return make_unique<VectorValue>(Vector3<float>(this->result).div(reinterpret_cast<const FloatValue*>(&rhs)->result));
      default:                                     throw harriet::Exception{"invalid input for binary operator '/'"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> VectorValue::computeEq (const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TVector:  return make_unique<BoolValue>(this->result == reinterpret_cast<const VectorValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '=='"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> VectorValue::computeNeq(const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TVector:  return make_unique<BoolValue>(this->result != reinterpret_cast<const VectorValue*>(&rhs)->result);
      default:                                     throw harriet::Exception{"invalid input for binary operator '!='"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> VectorValue::computeInv() const
{
                                                   return make_unique<VectorValue>(Vector3<float>(this->result).inverse());
}
//---------------------------------------------------------------------------
unique_ptr<Value> VectorValue::computeCast(harriet::VariableType resultType) const
{
   switch(resultType) {
      case harriet::VariableType::TInteger: return make_unique<IntegerValue>(this->result.x);
      case harriet::VariableType::TFloat:   return make_unique<FloatValue>(this->result.x);
      case harriet::VariableType::TBool:    return make_unique<BoolValue>(this->result.x!=0);
      case harriet::VariableType::TVector:  return make_unique<VectorValue>(this->result);
      default:                                     throw harriet::Exception{"invalid cast target: '" + harriet::typeToName(resultType) + "'"};
   }
}
//---------------------------------------------------------------------------
void CharacterValue::print(ostream& stream) const
{
   ostringstream os;
   for(auto iter : result)
      if(iter == '\0')
         break; else
         os << iter;
   stream << os.str();
}
//---------------------------------------------------------------------------
unique_ptr<Value> CharacterValue::evaluate() const
{
   return make_unique<CharacterValue>(result, result.size());
}
//---------------------------------------------------------------------------
unique_ptr<Value> CharacterValue::computeEq (const Value& rhs) const
{
   switch(rhs.getResultType()) {
      case harriet::VariableType::TCharacter:
      {
         auto other = reinterpret_cast<const CharacterValue*>(&rhs);
         return make_unique<BoolValue>(0==strncmp(result.data(), other->result.data(), max(result.size(), other->result.size())));
      }
      default:                                 throw harriet::Exception{"invalid input for binary operator '==' oh noze think of teh monkeys !!"};
   }
}
//---------------------------------------------------------------------------
unique_ptr<Value> CharacterValue::computeCast(harriet::VariableType resultType) const
{
   switch(resultType) {
      default:                                     throw harriet::Exception{"invalid cast target: '" + harriet::typeToName(resultType) + "'"};
   }
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
unique_ptr<Value> UnaryMinusOperator::evaluate(Environment& environment) const
{
   return child->evaluate(environment)->computeInv();
}
//---------------------------------------------------------------------------
unique_ptr<Value> NotOperator::evaluate(Environment& environment) const
{
   return child->evaluate(environment)->computeNot();
}
//---------------------------------------------------------------------------
unique_ptr<Value> CastOperator::evaluate(Environment& environment) const
{
   return child->evaluate(environment)->computeCast(getCastType());
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
   return lhs->evaluate(environment)->computeAdd(*rhs->evaluate(environment));
}
//---------------------------------------------------------------------------
unique_ptr<Value> MinusOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment)->computeSub(*rhs->evaluate(environment));
}
//---------------------------------------------------------------------------
unique_ptr<Value> MultiplicationOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment)->computeMul(*rhs->evaluate(environment));
}
//---------------------------------------------------------------------------
unique_ptr<Value> DivisionOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment)->computeDiv(*rhs->evaluate(environment));
}
//---------------------------------------------------------------------------
unique_ptr<Value> ModuloOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment)->computeMod(*rhs->evaluate(environment));
}
//---------------------------------------------------------------------------
unique_ptr<Value> ExponentiationOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment)->computeExp(*rhs->evaluate(environment));
}
//---------------------------------------------------------------------------
unique_ptr<Value> AndOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment)->computeAnd(*rhs->evaluate(environment));
}
//---------------------------------------------------------------------------
unique_ptr<Value> OrOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment)->computeOr (*rhs->evaluate(environment));
}
//---------------------------------------------------------------------------
unique_ptr<Value> GreaterOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment)->computeGt (*rhs->evaluate(environment));
}
//---------------------------------------------------------------------------
unique_ptr<Value> LessOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment)->computeLt (*rhs->evaluate(environment));
}
//---------------------------------------------------------------------------
unique_ptr<Value> GreaterEqualOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment)->computeGeq(*rhs->evaluate(environment));
}
//---------------------------------------------------------------------------
unique_ptr<Value> LessEqualOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment)->computeLeq(*rhs->evaluate(environment));
}
//---------------------------------------------------------------------------
unique_ptr<Value> EqualOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment)->computeEq(*rhs->evaluate(environment));
}
//---------------------------------------------------------------------------
unique_ptr<Value> NotEqualOperator::evaluate(Environment& environment) const
{
   return lhs->evaluate(environment)->computeNeq(*rhs->evaluate(environment));
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
      if(result->getResultType() != function->getArgumentType(i))
         throw harriet::Exception{"type missmatch in function '" + function->getName() + "' for argument '" + to_string(i) + "' unable to convert '" + harriet::typeToName(result->getResultType()) + "' to '" + harriet::typeToName(function->getArgumentType(i)) + "'"};
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
