#ifndef SCRIPTLANGUAGE_SCRIPTLANGUAGE_HPP_
#define SCRIPTLANGUAGE_SCRIPTLANGUAGE_HPP_
//---------------------------------------------------------------------------
#include "VariableType.hpp"
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
struct Exception : public std::exception {
   Exception(const std::string& message) : message(message) {}
   ~Exception() throw() {}
   const std::string message;
   virtual const char* what() const throw() {return message.c_str();}
};

// std::unique_ptr<Value> readValueContent(VariableType type, uint16_t length, const char* data) throw();
// void writeValueContent(const Value& value, char* data) throw();

bool isImplicitCastPossible(VariableType from, VariableType to) throw();

//---------------------------------------------------------------------------
} // end of namespace harriet
//---------------------------------------------------------------------------
#endif
