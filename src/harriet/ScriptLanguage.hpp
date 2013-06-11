#ifndef SCRIPTLANGUAGE_SCRIPTLANGUAGE_HPP_
#define SCRIPTLANGUAGE_SCRIPTLANGUAGE_HPP_
//---------------------------------------------------------------------------
#include <string>
#include <stdint.h>
#include <memory>
//---------------------------------------------------------------------------
// Harriet Script Language
// Copyright (c) 2012, 2013 Alexander van Renen (alexandervanrenen@gmail.com)
// See the file LICENSE.txt for copying permission.
//---------------------------------------------------------------------------
namespace harriet {
//---------------------------------------------------------------------------
class Value;
class Expression;
//---------------------------------------------------------------------------
/// other key words
const std::string kCastName = "cast";

/// variable types
const std::string kVariableInteger = "integer";
const std::string kVariableFloat = "float";
const std::string kVariableBool = "bool";
const std::string kVariableVector = "vector";
const std::string kVariableCharacter = "character";

/// boolean values
const std::string kTrue = "true";
const std::string kFalse = "false";

/// variable types
enum struct VariableType : uint8_t {TInteger, TFloat, TBool, TVector, TCharacter};

/// exceptions
struct Exception : public std::exception {
   Exception(const std::string& message) : message(message) {}
   ~Exception() throw() {}
   const std::string message;
   virtual const char* what() const throw() {return message.c_str();}
};

VariableType nameToType(const std::string& name) throw(Exception);
const std::string typeToName(VariableType type) throw();

uint16_t getDefaultLengthOfBinary(VariableType type);

std::unique_ptr<Value> createDefaultValue(VariableType type) throw();

std::unique_ptr<Value> readValueContent(VariableType type, uint16_t length, const char* data) throw();
void writeValueContent(const Value& value, char* data) throw();

bool isImplicitCastPossible(VariableType from, VariableType to) throw();
std::unique_ptr<Expression> createCast(std::unique_ptr<Expression> expression, harriet::VariableType resultType);

//---------------------------------------------------------------------------
} // end of namespace harriet
//---------------------------------------------------------------------------
#endif
