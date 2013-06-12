#include "Value.hpp"
#include "Utility.hpp"
#include <cassert>
#include <iostream>
#include <cstdlib>
#include <cstring>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace harriet {
//---------------------------------------------------------------------------
Value::Value(const VariableType& type)
: type(type)
{
   switch(type.type) {
      case VariableType::Type::TBool:
         data.vbool = false;
         return;
      case VariableType::Type::TInteger:
         data.vint = 0;
         return;
      case VariableType::Type::TFloat:
         data.vfloat = .0f;
         return;
      case VariableType::Type::TCharacter:
         data.vchar = static_cast<char*>(malloc(type.length));
         memset(data.vchar, '\0', type.length);
         return;
   }
   throw;
}
//---------------------------------------------------------------------------
Value::Value(const VariableType& type, const char* ptr)
: type(type)
{
   switch(type.type) {
      case VariableType::Type::TBool:
         data.vbool = reinterpret_cast<const bool&>(*ptr);
         return;
      case VariableType::Type::TInteger:
         data.vint = reinterpret_cast<const int32_t&>(*ptr);
         return;
      case VariableType::Type::TFloat:
         data.vfloat = reinterpret_cast<const float&>(*ptr);
         return;
      case VariableType::Type::TCharacter:
         data.vchar = static_cast<char*>(malloc(type.length));
         memset(data.vchar, '\0', type.length);
         memcpy(data.vchar, ptr, type.length);
         return;
   }
   throw;
}
//---------------------------------------------------------------------------
Value::Value(bool value, bool)
: type(VariableType::createBoolType())
{
   data.vbool = value;
}
//---------------------------------------------------------------------------
Value::Value(int32_t value, bool)
: type(VariableType::createIntegerType())
{
   data.vint = value;
}
//---------------------------------------------------------------------------
Value::Value(float value, bool)
: type(VariableType::createFloatType())
{
   data.vfloat = value;
}
//---------------------------------------------------------------------------
Value::Value(const string& value, int32_t max, bool)
: type(VariableType::createCharacterType(max))
{
   data.vchar = static_cast<char*>(malloc(type.length));
   memset(data.vchar, '\0', type.length);
   assert(value.size() <= type.length);
   memcpy(data.vchar, value.data(), min(static_cast<uint16_t>(value.size()), type.length));
}
//---------------------------------------------------------------------------
Value::~Value()
{
}
//---------------------------------------------------------------------------
void Value::marschall(char* ptr) const
{
   switch(type.type) {
      case VariableType::Type::TBool:
         reinterpret_cast<bool&>(*ptr) = data.vbool;
         return;
      case VariableType::Type::TInteger:
         reinterpret_cast<int32_t&>(*ptr) = data.vint;
         return;
      case VariableType::Type::TFloat:
         reinterpret_cast<float&>(*ptr) = data.vfloat;
         return;
      case VariableType::Type::TCharacter:
         memcpy(ptr, data.vchar, type.length);
         return;
   }
   throw;
}
//---------------------------------------------------------------------------
void Value::print(ostream& os) const
{
   os << *this;
}
//---------------------------------------------------------------------------
unique_ptr<Value> Value::evaluate(Environment&) const
{
   return make_unique<Value>(*this);
}
//---------------------------------------------------------------------------
unique_ptr<Value> Value::evaluate() const
{
   return make_unique<Value>(*this);
}
//---------------------------------------------------------------------------
vector<const Variable*> Value::getAllVariables() const
{
   return vector<const Variable*>();
}
//---------------------------------------------------------------------------
ExpressionType Value::getExpressionType() const
{
   return ExpressionType::TValue;
}
//---------------------------------------------------------------------------
ostream& operator<< (ostream& os, const Value& v)
{
   switch(v.type.type) {
      case VariableType::Type::TBool:
         return os << v.data.vbool;
      case VariableType::Type::TInteger:
         return os << v.data.vint;
      case VariableType::Type::TFloat:
         return os << v.data.vfloat;
      case VariableType::Type::TCharacter:
         return os << string(v.data.vchar, v.type.length);
      default:
         throw;
   }
}
//---------------------------------------------------------------------------
namespace {
void doError(const string& operatorSign, const Value& lhs, const Value& rhs) throw(harriet::Exception) { throw harriet::Exception{"binary operator '" + operatorSign + "' does not accept '" + lhs.type.str() + "' and '" + rhs.type.str() + "'"}; }
void doError(const string& operatorSign, const Value& lhs) throw(harriet::Exception) { throw harriet::Exception{"unary operator '" + operatorSign + "' does not accept '" + lhs.type.str() + "'"}; }
}
//---------------------------------------------------------------------------
Value Value::computeAdd(const Value& rhs) const
{
   switch(type.type) {
      case VariableType::Type::TBool:
         return Bool::computeAdd(*this, rhs);
      case VariableType::Type::TInteger:
         return Integer::computeAdd(*this, rhs);
      case VariableType::Type::TFloat:
         return Float::computeAdd(*this, rhs);
      case VariableType::Type::TCharacter:
         return Character::computeAdd(*this, rhs);
      default:
         doError("+" , *this, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::computeSub(const Value& rhs) const
{
   switch(type.type) {
      case VariableType::Type::TBool:
         return Bool::computeSub(*this, rhs);
      case VariableType::Type::TInteger:
         return Integer::computeSub(*this, rhs);
      case VariableType::Type::TFloat:
         return Float::computeSub(*this, rhs);
      case VariableType::Type::TCharacter:
         return Character::computeSub(*this, rhs);
      default:
         doError("-" , *this, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::computeMul(const Value& rhs) const
{
    switch(type.type) {
      case VariableType::Type::TBool:
         return Bool::computeMul(*this, rhs);
      case VariableType::Type::TInteger:
         return Integer::computeMul(*this, rhs);
      case VariableType::Type::TFloat:
         return Float::computeMul(*this, rhs);
      case VariableType::Type::TCharacter:
         return Character::computeMul(*this, rhs);
      default:
         doError("*" , *this, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::computeDiv(const Value& rhs) const
{
    switch(type.type) {
      case VariableType::Type::TBool:
         return Bool::computeDiv(*this, rhs);
      case VariableType::Type::TInteger:
         return Integer::computeDiv(*this, rhs);
      case VariableType::Type::TFloat:
         return Float::computeDiv(*this, rhs);
      case VariableType::Type::TCharacter:
         return Character::computeDiv(*this, rhs);
      default:
         doError("/" , *this, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::computeEq (const Value& rhs) const
{
   switch(type.type) {
      case VariableType::Type::TBool:
         return Bool::computeEq(*this, rhs);
      case VariableType::Type::TInteger:
         return Integer::computeEq(*this, rhs);
      case VariableType::Type::TFloat:
         return Float::computeEq(*this, rhs);
      case VariableType::Type::TCharacter:
         return Character::computeEq(*this, rhs);
      default:
         doError("==" , *this, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::Bool::computeAdd(const Value& lhs, const Value& rhs)
{
   doError("+", lhs, rhs);
   throw;
}
//---------------------------------------------------------------------------
Value Value::Bool::computeSub(const Value& lhs, const Value& rhs)
{
   doError("-", lhs, rhs);
   throw;
}
//---------------------------------------------------------------------------
Value Value::Bool::computeMul(const Value& lhs, const Value& rhs)
{
    doError("*", lhs, rhs);
    throw;
}
//---------------------------------------------------------------------------
Value Value::Bool::computeDiv(const Value& lhs, const Value& rhs)
{
    doError("/", lhs, rhs);
    throw;
}
//---------------------------------------------------------------------------
Value Value::Bool::computeEq (const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type) {
      case VariableType::Type::TBool:
         return Value(lhs.data.vbool==rhs.data.vbool);
      default:
         doError("==" , lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::Integer::computeAdd(const Value& lhs, const Value& rhs)
{
    switch(rhs.type.type){
        case VariableType::Type::TInteger:
            return Value(lhs.data.vint + rhs.data.vint);
        case VariableType::Type::TFloat:
            return Value(lhs.data.vint + rhs.data.vfloat);
        default:
            doError("+", lhs, rhs);
            throw;
    }
}
//---------------------------------------------------------------------------
Value Value::Integer::computeSub(const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type){
        case VariableType::Type::TInteger:
            return Value(lhs.data.vint - rhs.data.vint);
        case VariableType::Type::TFloat:
            return Value(lhs.data.vint - rhs.data.vfloat);
        default:
            doError("-", lhs, rhs);
            throw;
    }
}
//---------------------------------------------------------------------------
Value Value::Integer::computeMul(const Value& lhs, const Value& rhs)
{
    switch(rhs.type.type){
        case VariableType::Type::TInteger:
            return Value(lhs.data.vint * rhs.data.vint);
        case VariableType::Type::TFloat:
            return Value(lhs.data.vint * rhs.data.vfloat);
        default:
            doError("*", lhs, rhs);
            throw;
    }
}
//---------------------------------------------------------------------------
Value Value::Integer::computeDiv(const Value& lhs, const Value& rhs)
{
    switch(rhs.type.type){
        case VariableType::Type::TInteger:
            if(rhs.data.vint == 0) {
                doError("/ by 0", lhs, rhs);
                throw;
            }          
            else
                return Value(lhs.data.vint / rhs.data.vint);
        case VariableType::Type::TFloat:
            if(rhs.data.vfloat == 0.0f) {
                doError("/ by 0", lhs, rhs);
                throw;
            }
            else
                return Value(lhs.data.vint / rhs.data.vfloat);
        default:
            doError("/", lhs, rhs);
            throw;
    }
}
//---------------------------------------------------------------------------
Value Value::Integer::computeEq (const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type) {
      case VariableType::Type::TInteger:
         return Value(lhs.data.vint==rhs.data.vint);
      case VariableType::Type::TFloat:
         return Value(lhs.data.vint==rhs.data.vfloat);
      default:
         doError("==" , lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::Float::computeAdd(const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type){
        case VariableType::Type::TInteger:
            return Value(lhs.data.vfloat + rhs.data.vint);
        case VariableType::Type::TFloat:
            return Value(lhs.data.vfloat + rhs.data.vfloat);
        default:
            doError("+", lhs, rhs);
            throw;
    }
}
//---------------------------------------------------------------------------
Value Value::Float::computeSub(const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type){
        case VariableType::Type::TInteger:
            return Value(lhs.data.vfloat - rhs.data.vint);
        case VariableType::Type::TFloat:
            return Value(lhs.data.vfloat - rhs.data.vfloat);
        default:
            doError("-", lhs, rhs);
            throw;
    }
}
//---------------------------------------------------------------------------
Value Value::Float::computeMul(const Value& lhs, const Value& rhs)
{
    switch(rhs.type.type){
        case VariableType::Type::TInteger:
            return Value(lhs.data.vfloat * rhs.data.vint);
        case VariableType::Type::TFloat:
            return Value(lhs.data.vfloat * rhs.data.vfloat);
        default:
            doError("*", lhs, rhs);
            throw;
    }
}
//---------------------------------------------------------------------------
Value Value::Float::computeDiv(const Value& lhs, const Value& rhs)
{
    switch(rhs.type.type){
        case VariableType::Type::TInteger:
            if(rhs.data.vint == 0) {
                doError("/ by 0", lhs, rhs);
                throw;
            }          
            else
                return Value(lhs.data.vfloat / rhs.data.vint);
        case VariableType::Type::TFloat:
            if(rhs.data.vfloat == 0.0f) {
                doError("/ by 0", lhs, rhs);
                throw;
            }
            else
                return Value(lhs.data.vfloat / rhs.data.vfloat);
        default:
            doError("/", lhs, rhs);
            throw;
    }
}
//---------------------------------------------------------------------------
Value Value::Float::computeEq (const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type) {
      case VariableType::Type::TInteger:
         return Value(lhs.data.vfloat==rhs.data.vint);
      case VariableType::Type::TFloat:
         return Value(lhs.data.vfloat==rhs.data.vfloat);
      default:
         doError("==" , lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::Character::computeAdd(const Value& lhs, const Value& rhs)
{
    // TODO: auto conversion to char* to make appending of numbers possible
    switch(rhs.type.type) {
        case VariableType::Type::TCharacter: {
            char* res = static_cast<char*>(malloc(lhs.type.length + rhs.type.length));
            std::strncat(res, lhs.data.vchar, lhs.type.length);
            std::strncat(res, rhs.data.vchar, rhs.type.length);
            return Value(res);
        }
        default:
            doError("+", lhs, rhs);
            throw;
    }
}
//---------------------------------------------------------------------------
Value Value::Character::computeSub(const Value& lhs, const Value& rhs)
{
   doError("-", lhs, rhs);
   throw;
}
//---------------------------------------------------------------------------
Value Value::Character::computeMul(const Value& lhs, const Value& rhs)
{
    doError("*", lhs, rhs);
    throw;
}
//---------------------------------------------------------------------------
Value Value::Character::computeDiv(const Value& lhs, const Value& rhs)
{
    doError("/", lhs, rhs);
    throw;
}
//---------------------------------------------------------------------------
Value Value::Character::computeEq (const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type) {
      case VariableType::Type::TCharacter:
         return Value(strncmp(lhs.data.vchar, rhs.data.vchar, lhs.type.length==rhs.type.length?lhs.type.length:min(lhs.type.length, rhs.type.length)+1) == 0);
      default:
         doError("==" , lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------