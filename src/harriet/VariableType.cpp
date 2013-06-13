#include "VariableType.hpp"
#include "Value.hpp"
#include "Utility.hpp"
#include <sstream>
#include <ctype.h>
#include <cassert>
#include <istream>
#include <algorithm>
#include <cstring>
#include <math.h>
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

/// boolean values
static const std::string kTrue = "true";
static const std::string kFalse = "false";
}
//---------------------------------------------------------------------------
VariableType::VariableType()
: length(-1)
{
}
//---------------------------------------------------------------------------
VariableType::VariableType(const string& name, uint16_t length)
: length(length)
{
   if(name == kVariableBool) {
      type = Type::TBool;
      assert(length == sizeof(bool));
   } else if(name == kVariableInteger) {
      type = Type::TInteger;
      assert(length == sizeof(int32_t));
   } else if(name == kVariableFloat) {
      type = Type::TFloat;
      assert(length == sizeof(float));
   } else if(name == kVariableCharacter) {
      type = Type::TCharacter;
      assert(length >= sizeof(char));
   } else throw Exception{"invalid type name: " + name};
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
         assert(length >= sizeof(char));
         return;
   }
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
   assert(len > 0);
   return VariableType(VariableType::Type::TCharacter, len); 
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
         return kVariableCharacter;
   }
   throw Exception{"unreachable"};
}
//---------------------------------------------------------------------------
} // end of namespace harriet
//---------------------------------------------------------------------------
