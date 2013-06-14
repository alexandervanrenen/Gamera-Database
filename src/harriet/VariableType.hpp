#ifndef SCRIPTLANGUAGE_VARIABLETYPE_HPP_
#define SCRIPTLANGUAGE_VARIABLETYPE_HPP_
//---------------------------------------------------------------------------
#include <string>
#include <memory>
#include <ios>
//---------------------------------------------------------------------------
// Harriet Script Language
// Copyright (c) 2012, 2013 Alexander van Renen (alexandervanrenen@gmail.com)
// See the file LICENSE.txt for copying permission.
//---------------------------------------------------------------------------
namespace harriet {
//---------------------------------------------------------------------------
class VariableType {
public:
   /// Each type has a length
   enum struct Type : uint8_t {TBool, TInteger, TFloat, TCharacter};
   Type type;
   uint16_t length;

   /// Create a specific type
   explicit VariableType();
   static VariableType createBoolType();
   static VariableType createIntegerType();
   static VariableType createFloatType();
   static VariableType createCharacterType(uint16_t len);

   /// Convert to string or output to stream
   std::string str() const;
   friend std::ostream& operator<< (std::ostream& os, const VariableType& v);

   /// Compare to another type (length has to match)
   friend bool operator== (const VariableType& lhs, const VariableType& rhs);
   friend bool operator!= (const VariableType& lhs, const VariableType& rhs);
   
private:
   explicit VariableType(const std::string& name, uint16_t length);
   VariableType(Type type, uint16_t length);
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
