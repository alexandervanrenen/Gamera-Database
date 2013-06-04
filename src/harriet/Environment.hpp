#ifndef SCRIPTLANGUAGE_ENVIRONMENT_HPP_
#define SCRIPTLANGUAGE_ENVIRONMENT_HPP_
//---------------------------------------------------------------------------
#include <memory>
#include <string>
#include <vector>
//---------------------------------------------------------------------------
// Harriet Script Language
// Copyright (c) 2012, 2013 Alexander van Renen (alexandervanrenen@gmail.com)
// See the file LICENSE.txt for copying permission.
//---------------------------------------------------------------------------
namespace harriet {
//---------------------------------------------------------------------------
class Value;
class Function;
//---------------------------------------------------------------------------
class Environment {
public:
   /// ctor
   Environment(Environment* parentEnvironment = nullptr);
   ~Environment();

   /// variables
   void add(const std::string& identifier, std::unique_ptr<Value> value);
   void update(const std::string& identifier, std::unique_ptr<Value> value);
   const Value& read(const std::string& identifier) const;
   bool isInAnyScope(const std::string& identifier) const; // checks parents
   bool isInLocalScope(const std::string& identifier) const; // does not check parents

   /// functions
   void addFunction(std::unique_ptr<Function> function);
   bool hasFunction(const std::string& identifier);
   std::vector<const Function*> getFunction(const std::string& identifier); // all functions with same name
   const Function* getFunction(uint32_t id); // specific function

private:
   Environment* parent;
   std::vector<std::pair<std::string, std::unique_ptr<Value>>> data; // variables
   std::vector<std::unique_ptr<Function>> functions; // functions
};
//---------------------------------------------------------------------------
} // end of namespace harriet
//---------------------------------------------------------------------------
#endif
