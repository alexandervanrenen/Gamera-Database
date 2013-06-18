#ifndef SCRIPTLANGUAGE_VALUE_HPP_
#define SCRIPTLANGUAGE_VALUE_HPP_
//---------------------------------------------------------------------------
#include "VariableType.hpp"
#include <string>
#include <ios>
//---------------------------------------------------------------------------
// Harriet Script Language
// Copyright (c) 2012, 2013 Alexander van Renen (alexandervanrenen@gmail.com)
// See the file LICENSE.txt for copying permission.
//---------------------------------------------------------------------------
namespace harriet {
//---------------------------------------------------------------------------
/// The central value class of the database, used in every column
class Value {
   /// Don't allow direct construction
   Value(const VariableType& type);

public:
   /// Type of this value
   VariableType type;

   /// The actual data of this value
   union {
      bool vbool;
      int32_t vint;
      float vfloat;
      char* vchar;
   } data;
   bool isNull;

   /// Create different value types
   static Value createDefault(const VariableType& type);
   static Value createFromRecord(const VariableType& type, const char* ptr); // Copies the data
   static Value createBool(bool value, bool isNull = false);
   static Value createInteger(int32_t value, bool isNull = false);
   static Value createFloat(float value, bool isNull = false);
   static Value createCharacter(const std::string& value, uint16_t max, bool isNull = false);
   static Value createCharacter(char* value, uint16_t max, bool isNull = false); // Transfers ownership of the memory pointed to by value
   Value createCopy() const;
   Value(Value&& other);
   Value& operator=(Value&& other);
   virtual ~Value();

   /// Serialize the value to memory pointed by ptr
   void marschall(char* ptr) const;
   /// Convert any value into a string
   std::string str() const;
   /// Print to stream
   friend std::ostream& operator<< (std::ostream& os, const Value& v);

   /// Perform an operation -- creates new value
   Value computeAdd(const Value& rhs) const;
   Value computeSub(const Value& rhs) const;
   Value computeMul(const Value& rhs) const;
   Value computeDiv(const Value& rhs) const;
   Value computeEq (const Value& rhs) const;
   Value computeAnd(const Value& rhs) const;

private:
   /// Operations on boolean values
   struct Bool {
      static Value computeAdd(const Value& lhs, const Value& rhs);
      static Value computeSub(const Value& lhs, const Value& rhs);
      static Value computeMul(const Value& lhs, const Value& rhs);
      static Value computeDiv(const Value& lhs, const Value& rhs);
      static Value computeEq (const Value& lhs, const Value& rhs);
      static Value computeAnd(const Value& lhs, const Value& rhs);
   };

   /// Operations on integer values
   struct Integer {
      static Value computeAdd(const Value& lhs, const Value& rhs);
      static Value computeSub(const Value& lhs, const Value& rhs);
      static Value computeMul(const Value& lhs, const Value& rhs);
      static Value computeDiv(const Value& lhs, const Value& rhs);
      static Value computeEq (const Value& lhs, const Value& rhs);
      static Value computeAnd(const Value& lhs, const Value& rhs);
   };

   /// Operations on floating point values
   struct Float {
      static Value computeAdd(const Value& lhs, const Value& rhs);
      static Value computeSub(const Value& lhs, const Value& rhs);
      static Value computeMul(const Value& lhs, const Value& rhs);
      static Value computeDiv(const Value& lhs, const Value& rhs);
      static Value computeEq (const Value& lhs, const Value& rhs);
      static Value computeAnd(const Value& lhs, const Value& rhs);
   };

   /// Operations on fixed size character values
   struct Character {
      static Value computeAdd(const Value& lhs, const Value& rhs);
      static Value computeSub(const Value& lhs, const Value& rhs);
      static Value computeMul(const Value& lhs, const Value& rhs);
      static Value computeDiv(const Value& lhs, const Value& rhs);
      static Value computeEq (const Value& lhs, const Value& rhs);
      static Value computeAnd(const Value& lhs, const Value& rhs);
   };
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
