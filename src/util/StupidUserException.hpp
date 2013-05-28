#pragma once

#include <exception>
#include <string>

namespace dbi {

namespace util {

/// U got it ? U earned it !
struct StupidUserException : public std::exception {
   StupidUserException(const std::string& message) : message(message) {}
   ~StupidUserException() throw() {}
   const std::string message;
   virtual const char* what() const throw() {return message.c_str();}
};

}

}
