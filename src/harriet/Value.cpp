#include "Value.hpp"
#include "Utility.hpp"
#include <cassert>
#include <iostream>
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
         vbool = false;
         return;
      case VariableType::Type::TInteger:
         vint = 0;
         return;
      case VariableType::Type::TFloat:
         vfloat = .0f;
         return;
      case VariableType::Type::TCharacter:
         vchar = static_cast<char*>(malloc(type.length));
         memset(vchar, '\0', type.length);
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
         vbool = reinterpret_cast<const bool&>(*ptr);
         return;
      case VariableType::Type::TInteger:
         vint = reinterpret_cast<const int32_t&>(*ptr);
         return;
      case VariableType::Type::TFloat:
         vfloat = reinterpret_cast<const float&>(*ptr);
         return;
      case VariableType::Type::TCharacter:
         vchar = static_cast<char*>(malloc(type.length));
         memset(vchar, '\0', type.length);
         memcpy(vchar, ptr, type.length);
         return;
   }
   throw;
}
//---------------------------------------------------------------------------
Value::Value(bool value, bool)
: type(VariableType::Type::TBool, sizeof(bool))
{
   vbool = value;
}
//---------------------------------------------------------------------------
Value::Value(int32_t value, bool)
: type(VariableType::Type::TInteger, sizeof(int32_t))
{
   vint = value;
}
//---------------------------------------------------------------------------
Value::Value(float value, bool)
: type(VariableType::Type::TFloat, sizeof(int32_t))
{
   vfloat = value;
}
//---------------------------------------------------------------------------
Value::Value(const string& value, int32_t max, bool)
: type(VariableType::Type::TCharacter, max)
{
   vchar = static_cast<char*>(malloc(type.length));
   memset(vchar, '\0', type.length);
   assert(value.size() <= type.length);
   memcpy(vchar, value.data(), min(static_cast<uint16_t>(value.size()), type.length));
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
         reinterpret_cast<bool&>(*ptr) = vbool;
         return;
      case VariableType::Type::TInteger:
         reinterpret_cast<int32_t&>(*ptr) = vint;
         return;
      case VariableType::Type::TFloat:
         reinterpret_cast<float&>(*ptr) = vfloat;
         return;
      case VariableType::Type::TCharacter:
         memcpy(ptr, vchar, type.length);
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
         return os << v.vbool;
      case VariableType::Type::TInteger:
         return os << v.vint;
      case VariableType::Type::TFloat:
         return os << v.vfloat;
      case VariableType::Type::TCharacter:
         return os << string(v.vchar, v.type.length);
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

}
//---------------------------------------------------------------------------
Value Value::Bool::computeSub(const Value& lhs, const Value& rhs)
{

}
//---------------------------------------------------------------------------
Value Value::Bool::computeEq (const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type) {
      case VariableType::Type::TBool:
         return Value(lhs.vbool==rhs.vbool);
      default:
         doError("==" , lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::Integer::computeAdd(const Value& lhs, const Value& rhs)
{

}
//---------------------------------------------------------------------------
Value Value::Integer::computeSub(const Value& lhs, const Value& rhs)
{

}
//---------------------------------------------------------------------------
Value Value::Integer::computeEq (const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type) {
      case VariableType::Type::TInteger:
         return Value(lhs.vint==rhs.vint);
      case VariableType::Type::TFloat:
         return Value(lhs.vint==rhs.vfloat);
      default:
         doError("==" , lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::Float::computeAdd(const Value& lhs, const Value& rhs)
{

}
//---------------------------------------------------------------------------
Value Value::Float::computeSub(const Value& lhs, const Value& rhs)
{

}
//---------------------------------------------------------------------------
Value Value::Float::computeEq (const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type) {
      case VariableType::Type::TInteger:
         return Value(lhs.vint==rhs.vint);
      case VariableType::Type::TFloat:
         return Value(lhs.vint==rhs.vfloat);
      default:
         doError("==" , lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::Character::computeAdd(const Value& lhs, const Value& rhs)
{

}
//---------------------------------------------------------------------------
Value Value::Character::computeSub(const Value& lhs, const Value& rhs)
{

}
//---------------------------------------------------------------------------
Value Value::Character::computeEq (const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type) {
      case VariableType::Type::TCharacter:
         return Value(strncmp(lhs.vchar, rhs.vchar, lhs.type.length==rhs.type.length?lhs.type.length:min(lhs.type.length, rhs.type.length)+1) == 0);
      default:
         doError("==" , lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------