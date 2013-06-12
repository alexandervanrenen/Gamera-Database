#ifndef SCRIPTLANGUAGE_VALUE_HPP_
#define SCRIPTLANGUAGE_VALUE_HPP_
//---------------------------------------------------------------------------
#include "Expression.hpp"
#include <string>
#include <ios>
//---------------------------------------------------------------------------
// Harriet Script Language
// Copyright (c) 2012, 2013 Alexander van Renen (alexandervanrenen@gmail.com)
// See the file LICENSE.txt for copying permission.
//---------------------------------------------------------------------------
namespace harriet {
//---------------------------------------------------------------------------
class Value : public Expression {
public:
   VariableType type;

   union {
      bool vbool;
      int32_t vint;
      float vfloat;
      char* vchar;
   };
   bool isNull;

   explicit Value(const VariableType& type);
   explicit Value(const VariableType& type, const char* ptr);
   explicit Value(bool value, bool isNull = false);
   explicit Value(int32_t value, bool isNull = false);
   explicit Value(float value, bool isNull = false);
   explicit Value(const std::string& value, int32_t max, bool isNull = false);
   virtual ~Value();

   void marschall(char* ptr) const;

   virtual void print(std::ostream&) const;
   virtual std::unique_ptr<Value> evaluate(Environment& environment) const;
   virtual std::unique_ptr<Value> evaluate() const;
   virtual std::vector<const Variable*> getAllVariables() const;
   virtual ExpressionType getExpressionType() const;
   friend std::ostream& operator<< (std::ostream& os, const Value& v);

   Value computeAdd(const Value& rhs) const;
   Value computeSub(const Value& rhs) const;
   Value computeEq (const Value& rhs) const;

private:
   struct Bool {
      static Value computeAdd(const Value& lhs, const Value& rhs);
      static Value computeSub(const Value& lhs, const Value& rhs);
      static Value computeEq (const Value& lhs, const Value& rhs);
   };

   struct Integer {
      static Value computeAdd(const Value& lhs, const Value& rhs);
      static Value computeSub(const Value& lhs, const Value& rhs);
      static Value computeEq (const Value& lhs, const Value& rhs);
   };

   struct Float {
      static Value computeAdd(const Value& lhs, const Value& rhs);
      static Value computeSub(const Value& lhs, const Value& rhs);
      static Value computeEq (const Value& lhs, const Value& rhs);
   };

   struct Character {
      static Value computeAdd(const Value& lhs, const Value& rhs);
      static Value computeSub(const Value& lhs, const Value& rhs);
      static Value computeEq (const Value& lhs, const Value& rhs);
   };
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
