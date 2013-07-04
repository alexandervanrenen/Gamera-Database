#include "Value.hpp"
#include "ScriptLanguage.hpp"
#include "Utility.hpp"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace harriet {
//---------------------------------------------------------------------------
Value::Value(const VariableType& type)
: type(type)
{
}
//---------------------------------------------------------------------------
Value Value::createDefault(const VariableType& type)
{
   Value result(type);
   switch(type.type) {
      case VariableType::Type::TBool:
         result.data.vbool = false;
         return result;
      case VariableType::Type::TInteger:
         result.data.vint = 0;
         return result;
      case VariableType::Type::TFloat:
         result.data.vfloat = .0f;
         return result;
      case VariableType::Type::TCharacter:
         result.data.vchar = static_cast<char*>(malloc(type.length));
         memset(result.data.vchar, '\0', type.length);
         return result;
      case VariableType::Type::TUndefined:
         result.data.vchar = nullptr;
         return result;
   }
   throw "unreachable";
}
//---------------------------------------------------------------------------
Value Value::createFromRecord(const VariableType& type, const char* ptr)
{
   Value result(type);
   switch(type.type) {
      case VariableType::Type::TBool:
         result.data.vbool = reinterpret_cast<const bool&>(*ptr);
         return result;
      case VariableType::Type::TInteger:
         result.data.vint = reinterpret_cast<const int32_t&>(*ptr);
         return result;
      case VariableType::Type::TFloat:
         result.data.vfloat = reinterpret_cast<const float&>(*ptr);
         return result;
      case VariableType::Type::TCharacter:
         result.data.vchar = static_cast<char*>(malloc(type.length));
         memset(result.data.vchar, '\0', type.length);
         memcpy(result.data.vchar, ptr, type.length);
         return result;
      default:
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::createBool(bool value, bool)
{
   Value result(VariableType::createBoolType());
   result.data.vbool = value;
   return result;
}
//---------------------------------------------------------------------------
Value Value::createInteger(int32_t value, bool)
{
   Value result(VariableType::createIntegerType());
   result.data.vint = value;
   return result;
}
//---------------------------------------------------------------------------
Value Value::createFloat(float value, bool)
{
   Value result(VariableType::createFloatType());
   result.data.vfloat = value;
   return result;
}
//---------------------------------------------------------------------------
Value Value::createCharacter(const string& value, uint16_t max, bool)
{
   assert(value.size() <= max);
   Value result(VariableType::createCharacterType(max));
   result.data.vchar = static_cast<char*>(malloc(max));
   memset(result.data.vchar, '\0', max);
   memcpy(result.data.vchar, value.data(), value.size());
   return result;
}
//---------------------------------------------------------------------------
Value Value::createCharacter(string&& val, bool)
{
   auto value = move(val);
   Value result(VariableType::createCharacterType(value.size()));
   result.data.vchar = static_cast<char*>(malloc(value.size()));
   memset(result.data.vchar, '\0', value.size()); // TODO: dont copy put figure out how to extract the memory directly from the string
   memcpy(result.data.vchar, value.data(), value.size());
   return result;
}
//---------------------------------------------------------------------------
Value Value::createCharacter(char* value, uint16_t max, bool)
{
   Value result(VariableType::createCharacterType(max));
   result.data.vchar = value;
   return result;
}
//---------------------------------------------------------------------------
Value Value::createCopy() const
{
   Value result(type);
   if(type.type == VariableType::Type::TCharacter) {
      // Got to copy pointer
      result.data.vchar = static_cast<char*>(malloc(type.length));
      memcpy(result.data.vchar, data.vchar, type.length);
   } else {
      result.data = data;
   }
   return result;
}
//---------------------------------------------------------------------------
Value::Value(Value&& other)
{
   // Destroy this values
   if(type.type==VariableType::Type::TCharacter && data.vchar!=nullptr)
      free(data.vchar);
   // Move data from other value into this value
   type = other.type;
   data = other.data;
   other.data.vchar = nullptr;
}
//---------------------------------------------------------------------------
Value& Value::operator=(Value&& other)
{
   // Destroy this values
   if(type.type==VariableType::Type::TCharacter && data.vchar!=nullptr)
      free(data.vchar);
   // Move data from other value into this value
   type = other.type;
   data = other.data;
   other.data.vchar = nullptr;
   return *this;
}
//---------------------------------------------------------------------------
Value::~Value()
{
   if(type.type==VariableType::Type::TCharacter && data.vchar!=nullptr)
      free(data.vchar);
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
      default:
         throw;
   }
}
//---------------------------------------------------------------------------
string Value::str() const
{
   assert(type.type != VariableType::Type::TUndefined);
   ostringstream os;
   os << *this;
   return os.str();
}
//---------------------------------------------------------------------------
ostream& operator<< (ostream& os, const Value& v)
{
   assert(v.type.type != VariableType::Type::TUndefined);
   switch(v.type.type) {
      case VariableType::Type::TBool:
         return os << v.data.vbool;
      case VariableType::Type::TInteger:
         return os << v.data.vint;
      case VariableType::Type::TFloat:
         return os << v.data.vfloat;
      case VariableType::Type::TCharacter:
         return os << string(v.data.vchar, strnlen(v.data.vchar, v.type.length));
      default:
         throw;
   }
}
//---------------------------------------------------------------------------
namespace {
void doError(const string& operatorSign, const Value& lhs, const Value& rhs) throw(Exception) { throw Exception{"binary operator '" + operatorSign + "' does not accept '" + lhs.type.str() + "' and '" + rhs.type.str() + "'"}; }
// void doError(const string& operatorSign, const Value& lhs) throw(Exception) { throw Exception{"unary operator '" + operatorSign + "' does not accept '" + lhs.type.str() + "'"}; }
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
Value Value::computeNeq(const Value& rhs) const
{
   try {
      auto val = computeEq(rhs);
      val.data.vbool = !val.data.vbool;
      return val;
   } catch(Exception& e) {
      doError("!=" , *this, rhs);
      throw;
   }
}
//---------------------------------------------------------------------------
Value Value::computeAnd (const Value& rhs) const
{
   switch(type.type) {
      case VariableType::Type::TBool:
         return Bool::computeAnd(*this, rhs);
      case VariableType::Type::TInteger:
         return Integer::computeAnd(*this, rhs);
      case VariableType::Type::TFloat:
         return Float::computeAnd(*this, rhs);
      case VariableType::Type::TCharacter:
         return Character::computeAnd(*this, rhs);
      default:
         doError("&" , *this, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::computeLeq(const Value& rhs) const
{
   switch(type.type) {
      case VariableType::Type::TBool:
         return Bool::computeLeq(*this, rhs);
      case VariableType::Type::TInteger:
         return Integer::computeLeq(*this, rhs);
      case VariableType::Type::TFloat:
         return Float::computeLeq(*this, rhs);
      case VariableType::Type::TCharacter:
         return Character::computeLeq(*this, rhs);
      default:
         doError("<=" , *this, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::computeLt(const Value& rhs) const
{
   try {
      auto val = computeGeq(rhs);
      val.data.vbool = !val.data.vbool;
      return val;
   } catch(Exception& e) {
      doError("<" , *this, rhs);
      throw;
   }
}
//---------------------------------------------------------------------------
Value Value::computeGeq(const Value& rhs) const
{
   switch(type.type) {
      case VariableType::Type::TBool:
         return Bool::computeGeq(*this, rhs);
      case VariableType::Type::TInteger:
         return Integer::computeGeq(*this, rhs);
      case VariableType::Type::TFloat:
         return Float::computeGeq(*this, rhs);
      case VariableType::Type::TCharacter:
         return Character::computeGeq(*this, rhs);
      default:
         doError(">=" , *this, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::computeGt(const Value& rhs) const
{
   try {
      auto val = computeLeq(rhs);
      val.data.vbool = !val.data.vbool;
      return val;
   } catch(Exception& e) {
      doError(">" , *this, rhs);
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
         return createBool(lhs.data.vbool==rhs.data.vbool);
      default:
         doError("==" , lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::Bool::computeAnd(const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type) {
      case VariableType::Type::TBool:
         return createBool(lhs.data.vbool && rhs.data.vbool);
      default:
         doError("&", lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::Bool::computeLeq(const Value& lhs, const Value& rhs)
{
   doError("<=", lhs, rhs);
   throw;
}
//---------------------------------------------------------------------------
Value Value::Bool::computeGeq(const Value& lhs, const Value& rhs)
{
   doError(">=", lhs, rhs);
   throw;
}
//---------------------------------------------------------------------------
Value Value::Integer::computeAdd(const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type){
      case VariableType::Type::TInteger:
         return createInteger(lhs.data.vint + rhs.data.vint);
      case VariableType::Type::TFloat:
         return createFloat(lhs.data.vint + rhs.data.vfloat);
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
         return createInteger(lhs.data.vint - rhs.data.vint);
      case VariableType::Type::TFloat:
         return createFloat(lhs.data.vint - rhs.data.vfloat);
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
         return createInteger(lhs.data.vint * rhs.data.vint);
      case VariableType::Type::TFloat:
         return createFloat(lhs.data.vint * rhs.data.vfloat);
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
            return createInteger(lhs.data.vint / rhs.data.vint);
      case VariableType::Type::TFloat:
         if(rhs.data.vfloat == 0.0f) {
            doError("/ by 0", lhs, rhs);
            throw;
         }
         else
            return createFloat(lhs.data.vint / rhs.data.vfloat);
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
         return createBool(lhs.data.vint==rhs.data.vint);
      case VariableType::Type::TFloat:
         return createBool(lhs.data.vint==rhs.data.vfloat);
      default:
         doError("==" , lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::Integer::computeAnd(const Value& lhs, const Value& rhs)
{
   doError("&", lhs, rhs);
   throw;
}
//---------------------------------------------------------------------------
Value Value::Integer::computeLeq(const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type) {
      case VariableType::Type::TInteger:
         return createBool(lhs.data.vint <= rhs.data.vint);
      case VariableType::Type::TFloat:
         return createBool(lhs.data.vint <= rhs.data.vfloat);
      default:
         doError("<=", lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::Integer::computeGeq(const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type) {
      case VariableType::Type::TInteger:
         return createBool(lhs.data.vint >= rhs.data.vint);
      case VariableType::Type::TFloat:
         return createBool(lhs.data.vint >= rhs.data.vfloat);
      default:
         doError(">=", lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::Float::computeAdd(const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type){
      case VariableType::Type::TInteger:
         return createFloat(lhs.data.vfloat + rhs.data.vint);
      case VariableType::Type::TFloat:
         return createFloat(lhs.data.vfloat + rhs.data.vfloat);
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
         return createFloat(lhs.data.vfloat - rhs.data.vint);
      case VariableType::Type::TFloat:
         return createFloat(lhs.data.vfloat - rhs.data.vfloat);
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
         return createFloat(lhs.data.vfloat * rhs.data.vint);
      case VariableType::Type::TFloat:
         return createFloat(lhs.data.vfloat * rhs.data.vfloat);
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
            return createFloat(lhs.data.vfloat / rhs.data.vint);
      case VariableType::Type::TFloat:
         if(rhs.data.vfloat == 0.0f) {
            doError("/ by 0", lhs, rhs);
            throw;
         }
         else
            return createFloat(lhs.data.vfloat / rhs.data.vfloat);
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
         return createBool(lhs.data.vfloat==rhs.data.vint);
      case VariableType::Type::TFloat:
         return createBool(lhs.data.vfloat==rhs.data.vfloat);
      default:
         doError("==" , lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::Float::computeAnd(const Value& lhs, const Value& rhs)
{
   doError("&", lhs, rhs);
   throw;
}
//---------------------------------------------------------------------------
Value Value::Float::computeLeq(const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type) {
      case VariableType::Type::TInteger:
         return createBool(lhs.data.vfloat <= rhs.data.vint);
      case VariableType::Type::TFloat: 
         return createBool(lhs.data.vfloat <= rhs.data.vfloat);
      default:
         doError("<=" , lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::Float::computeGeq(const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type) {
      case VariableType::Type::TInteger:
         return createBool(lhs.data.vfloat >= rhs.data.vint);
      case VariableType::Type::TFloat: 
         return createBool(lhs.data.vfloat >= rhs.data.vfloat);
      default:
         doError(">=" , lhs, rhs);
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
         char* writeHead = res;
         // Copy right hand side
         for(char* readHead=lhs.data.vchar; readHead!=lhs.data.vchar+lhs.type.length&&*readHead!='\0';)
            *(writeHead++) = *(readHead++);
         // Copy left hand side
         for(char* readHead=rhs.data.vchar; readHead!=rhs.data.vchar+rhs.type.length&&*readHead!='\0';)
            *(writeHead++) = *(readHead++);
         // Fill with \0
         while(writeHead != res+lhs.type.length+rhs.type.length)
            *(writeHead++) = '\0';
         return createCharacter(res, lhs.type.length + rhs.type.length);
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
      case VariableType::Type::TCharacter: {
         bool isContentEq = 0==memcmp(lhs.data.vchar, rhs.data.vchar, min(lhs.type.length, rhs.type.length));
         if(lhs.type.length == rhs.type.length) {
            return createBool(isContentEq);
         } else if(lhs.type.length < rhs.type.length) {
            return createBool(rhs.data.vchar[lhs.type.length]=='\0' && isContentEq);
         } else {
            return createBool(lhs.data.vchar[rhs.type.length]=='\0' && isContentEq);
         }
      }
      default:
         doError("==" , lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::Character::computeAnd(const Value& lhs, const Value& rhs)
{
   doError("&", lhs, rhs);
   throw;
}
//---------------------------------------------------------------------------
Value Value::Character::computeLeq(const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type) {
      case VariableType::Type::TCharacter: {
         auto cmpVal = memcmp(lhs.data.vchar, rhs.data.vchar, min(lhs.type.length, rhs.type.length));
         if(cmpVal != 0) {
            return createBool(cmpVal < 0 );
         } else {
            // inputs are equals within their minimum size and the longer one has a termination symbol at (minSize + 1)
            if((lhs.type.length < rhs.type.length && rhs.data.vchar[lhs.type.length]=='\0') ||
               (lhs.type.length > rhs.type.length && lhs.data.vchar[rhs.type.length]=='\0')) {
               return createBool(true);
            } else {
               return createBool(lhs.type.length <= rhs.type.length);
            }
         }
      }
      default:
         doError("<=", lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
Value Value::Character::computeGeq(const Value& lhs, const Value& rhs)
{
   switch(rhs.type.type) {
      case VariableType::Type::TCharacter: {
         auto cmpVal =  memcmp(lhs.data.vchar, rhs.data.vchar, min(lhs.type.length, rhs.type.length));
         if(cmpVal != 0) {
            return createBool(cmpVal > 0 );
         } else {
            // inputs are equals within their minimum size and the longer one has a termination symbol at (minSize + 1)
            if((lhs.type.length < rhs.type.length && rhs.data.vchar[lhs.type.length]=='\0') ||
               (lhs.type.length > rhs.type.length && lhs.data.vchar[rhs.type.length]=='\0')) {
               return createBool(true);
            } else {
               return createBool(lhs.type.length >= rhs.type.length);
            }
         }
      }
      default:
         doError(">=", lhs, rhs);
         throw;
   }
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
