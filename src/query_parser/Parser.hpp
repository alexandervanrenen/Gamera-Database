#pragma once

#include <string>
#include <exception>
#include <memory>

namespace dbi {

namespace script {

class RootStatement;

struct ParserException : public std::exception {
   ParserException(const std::string& message, uint32_t line, uint32_t column) : message(message), line(line), column(column) {}
   ~ParserException() throw() {}
   const std::string message;
   const uint32_t line;
   const uint32_t column;
   virtual const char* what() const throw() {return (message + " (line: " + std::to_string(line) + "; column: " + std::to_string(column) + ")").c_str();}
};

std::unique_ptr<RootStatement> parse(const std::string& query) throw(ParserException);
}

}
