#ifndef SCRIPTLANGUAGE_VARIABLETYPE_HPP_
#define SCRIPTLANGUAGE_VARIABLETYPE_HPP_
//---------------------------------------------------------------------------
#include <ios>
#include <memory>
#include <string>
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
   enum struct Type : uint8_t {TBool, TInteger, TFloat, TCharacter, TUndefined}; // Undefined is needed only for coding. There are some places where we create a value, which has no type yet.
   Type type;
   uint16_t length;

   /// Create a specific type
   VariableType(); // Creates undefined type
   static VariableType createBoolType();
   static VariableType createIntegerType();
   static VariableType createFloatType();
   static VariableType createCharacterType(uint16_t len);
   static VariableType createUndefinedType();

   /// Convert to string or output to stream
   std::string str() const;
   friend std::ostream& operator<< (std::ostream& os, const VariableType& v);

   /// Compare to another type (length has to match)
   friend bool operator== (const VariableType& lhs, const VariableType& rhs);
   friend bool operator!= (const VariableType& lhs, const VariableType& rhs);
   
private:
   explicit VariableType(Type type, uint16_t length);
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
