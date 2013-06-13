#ifndef SCRIPTLANGUAGE_VARIABLETYPE_HPP_
#define SCRIPTLANGUAGE_VARIABLETYPE_HPP_
//---------------------------------------------------------------------------
#include <string>
#include <memory>
#include <iostream> // AAA
//---------------------------------------------------------------------------
// Harriet Script Language
// Copyright (c) 2012, 2013 Alexander van Renen (alexandervanrenen@gmail.com)
// See the file LICENSE.txt for copying permission.
//---------------------------------------------------------------------------
namespace harriet {
//---------------------------------------------------------------------------
class Value;
//---------------------------------------------------------------------------
struct VariableType {
   enum struct Type : uint8_t {TBool, TInteger, TFloat, TCharacter};
   Type type;
   uint16_t length;

   explicit VariableType();
   explicit VariableType(const std::string& name, uint16_t length);

   std::string str() const;
   
   static VariableType createBoolType();
   static VariableType createIntegerType();
   static VariableType createFloatType();
   static VariableType createCharacterType(uint16_t len);
   
   friend bool operator== (const VariableType& lhs, const VariableType& rhs) {return lhs.type==rhs.type && lhs.length==rhs.length;}
   friend bool operator!= (const VariableType& lhs, const VariableType& rhs) {return lhs.type!=rhs.type || lhs.length!=rhs.length;}
   friend std::ostream& operator<< (std::ostream& os, const VariableType& v) {return os << v.str() << " " << v.length;}
   
   private:
       VariableType(Type type, uint16_t length);
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
