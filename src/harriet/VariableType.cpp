#include "VariableType.hpp"
#include "ScriptLanguage.hpp"
#include <cassert>
#include <sstream>
//---------------------------------------------------------------------------
// Harriet Script Language
// Copyright (c) 2013 Alexander van Renen (alexandervanrenen@gmail.com)
// See the file LICENSE.txt for copying permission.
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace harriet {
//---------------------------------------------------------------------------
namespace {
/// variable types
static const std::string kVariableInteger = "integer";
static const std::string kVariableFloat = "float";
static const std::string kVariableBool = "bool";
static const std::string kVariableCharacter = "character";
static const std::string kVariableUndefined = "undefined";
}
//---------------------------------------------------------------------------
VariableType::VariableType()
: type(Type::TUndefined)
, length(0)
{
}
//---------------------------------------------------------------------------
VariableType VariableType::createBoolType()
{
    return VariableType(VariableType::Type::TBool, sizeof(bool));
}
//---------------------------------------------------------------------------
VariableType VariableType::createIntegerType()
{
    return VariableType(VariableType::Type::TInteger, sizeof(int32_t));
}
//---------------------------------------------------------------------------
VariableType VariableType::createFloatType()
{
    return VariableType(VariableType::Type::TFloat, sizeof(float));
}
//---------------------------------------------------------------------------
VariableType VariableType::createCharacterType(uint16_t len)
{
   return VariableType(VariableType::Type::TCharacter, len);
}
//---------------------------------------------------------------------------
VariableType VariableType::createUndefinedType()
{
   return VariableType(VariableType::Type::TUndefined, 0);
}
//---------------------------------------------------------------------------
string VariableType::str() const
{
   switch(type) {
      case Type::TBool:
         return kVariableBool;
      case Type::TInteger:
         return kVariableInteger;
      case Type::TFloat:
         return kVariableFloat;
      case Type::TCharacter:
         return kVariableCharacter + " (" + to_string(length) + ")";
      case Type::TUndefined:
         return kVariableUndefined;
   }
   throw Exception{"unreachable"};
}
//---------------------------------------------------------------------------
std::ostream& operator<< (std::ostream& os, const VariableType& v)
{
   return os << v.str() << " " << v.length;
}
//---------------------------------------------------------------------------
bool operator== (const VariableType& lhs, const VariableType& rhs)
{
   return lhs.type==rhs.type && lhs.length==rhs.length;
}
//---------------------------------------------------------------------------
bool operator!= (const VariableType& lhs, const VariableType& rhs)
{
   return lhs.type!=rhs.type || lhs.length!=rhs.length;
}
//---------------------------------------------------------------------------
VariableType::VariableType(Type type, uint16_t length)
: type(type)
, length(length)
{
   switch(type) {
      case Type::TBool:
         assert(length == sizeof(bool));
         return;
      case Type::TInteger:
         assert(length == sizeof(int32_t));
         return;
      case Type::TFloat:
         assert(length == sizeof(float));
         return;
      case Type::TCharacter:
         return;
      case Type::TUndefined:
         assert(length == 0);
         return;
   }
}
//---------------------------------------------------------------------------
uint32_t VariableType::getMaxValuesASCIIRepresentationSize() const
{
   switch(type) {
      case Type::TBool:
         return 5; // false
      case Type::TInteger:
         assert(length == sizeof(int32_t));
         return 11; // -10^10
      case Type::TFloat:
         assert(length == sizeof(float));
         return 12; // Don't know
      case Type::TCharacter:
         return length;
      case Type::TUndefined:
         throw;
   }
   throw;
}
//---------------------------------------------------------------------------
} // end of namespace harriet
//---------------------------------------------------------------------------
