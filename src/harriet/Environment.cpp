#include "ScriptLanguage.hpp"
#include "Environment.hpp"
#include "Expression.hpp"
#include "Function.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
//---------------------------------------------------------------------------
// Harriet Script Language
// Copyright (c) 2013 Alexander van Renen (alexandervanrenen@gmail.com)
// See the file LICENSE.txt for copying permission.
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace harriet {
//---------------------------------------------------------------------------
Environment::Environment(Environment* parentEnvironment)
: parent(parentEnvironment)
{
}
//---------------------------------------------------------------------------
Environment::~Environment()
{
}
//---------------------------------------------------------------------------
void Environment::add(const string& identifier, unique_ptr<Value> value)
{
   assert(none_of(data.begin(), data.end(), [&identifier](const pair<string,unique_ptr<Value>>& iter){return iter.first==identifier;}));
   data.push_back(make_pair(identifier, ::move(value)));
}
//---------------------------------------------------------------------------
void Environment::update(const string& identifier, unique_ptr<Value> value)
{
   assert(isInAnyScope(identifier));
   for(auto& iter : data)
      if(iter.first == identifier) {
         iter.second = ::move(value);
         return;
      }
   parent->update(identifier, ::move(value));
}
//---------------------------------------------------------------------------
const Value& Environment::read(const string& identifier) const
{
   assert(isInAnyScope(identifier));
   for(auto& iter : data)
      if(iter.first == identifier)
         return *iter.second;
   return parent->read(identifier);
}
//---------------------------------------------------------------------------
bool Environment::isInAnyScope(const string& identifier) const
{
   for(auto& iter : data)
      if(iter.first == identifier)
         return true;
   if(parent!=nullptr)
      return parent->isInAnyScope(identifier); else
      return false;
}
//---------------------------------------------------------------------------
bool Environment::isInLocalScope(const string& identifier) const
{
   for(auto& iter : data)
      if(iter.first == identifier)
         return true;
   return false;
}
//---------------------------------------------------------------------------
void Environment::addFunction(unique_ptr<Function> function)
{
   // ensures: functions.name equal => functions.returntype equal and functions.name equal => functions.arguments !equal
   assert(none_of(functions.begin(), functions.end(), [&function](unique_ptr<Function>& iter) {
      if(iter->getName()!=function->getName())
         return false;
      if(iter->getResultType()!=function->getResultType())
         return true;
      if(iter->getArgumentCount()!=function->getArgumentCount())
         return false;
      for(uint32_t i=0; i<iter->getArgumentCount(); i++)
         if(iter->getArgumentType(i) != function->getArgumentType(i))
            return false;
      return true;
   }));

   functions.push_back(::move(function));
}
//---------------------------------------------------------------------------
bool Environment::hasFunction(const string& identifier)
{
   for(auto& iter : functions)
      if(iter->getName() == identifier)
         return true;
   if(parent!=nullptr)
      return parent->hasFunction(identifier); else
      return false;
}
//---------------------------------------------------------------------------
vector<const Function*> Environment::getFunction(const string& identifier)
{
   assert(hasFunction(identifier));
   vector<const Function*> result;
   for(auto& iter : functions)
      if(iter->getName() == identifier)
         result.push_back(iter.get());

   if(parent!=nullptr) {
      auto parentResult = parent->getFunction(identifier);
      result.insert(result.end(), parentResult.begin(), parentResult.end());
   }
   return result;
}
//---------------------------------------------------------------------------
const Function* Environment::getFunction(uint32_t id)
{
   for(auto& iter : functions)
      if(iter->getId() == id)
         return iter.get();

   if(parent!=nullptr)
      return parent->getFunction(id);

   throw;
}
//---------------------------------------------------------------------------
} // end of namespace harriet
//---------------------------------------------------------------------------
