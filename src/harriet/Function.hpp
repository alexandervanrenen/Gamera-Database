#ifndef SCRIPTLANGUAGE_FUNCTION_HPP_
#define SCRIPTLANGUAGE_FUNCTION_HPP_
//---------------------------------------------------------------------------
#include "ScriptLanguage.hpp"
#include <memory>
#include <vector>
#include <string>
//---------------------------------------------------------------------------
// Harriet Script Language
// Copyright (c) 2012, 2013 Alexander van Renen (alexandervanrenen@gmail.com)
// See the file LICENSE.txt for copying permission.
//---------------------------------------------------------------------------
namespace harriet {
//---------------------------------------------------------------------------
class Environment;
class Value;
//---------------------------------------------------------------------------
class Function {
public:
   /// ctor for build in function
   Function(const std::string& name, uint32_t id, std::function<std::unique_ptr<Value>(std::vector<std::unique_ptr<Value>>&, Environment&)> func, std::vector<harriet::VariableType> argumentTypes, harriet::VariableType resultType);
   /// dtor
   ~Function();

   /// run function
   std::unique_ptr<Value> execute(std::vector<std::unique_ptr<Value>>& argv, Environment& env) const;

   /// access properties
   harriet::VariableType getResultType() const;
   uint32_t getArgumentCount() const;
   harriet::VariableType getArgumentType(uint32_t index) const;
   const std::string& getName() const;
   uint32_t getId() const {return id;}

   /// helper
   const std::string getFunctionHeader() const;

private:
   const std::string name;
   const uint32_t id;
   std::function<std::unique_ptr<Value>(std::vector<std::unique_ptr<Value>>&, Environment&)> func;
   std::vector<std::pair<harriet::VariableType, std::string>> arguments;
   harriet::VariableType resultType;
};
//---------------------------------------------------------------------------
} // end of namespace harriet
//---------------------------------------------------------------------------
#endif
