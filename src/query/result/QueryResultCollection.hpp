#pragma once

#include <vector>
#include <chrono>
#include <string>
#include <memory>

namespace harriet { class Value; }

namespace dbi {

class QueryResult;

class QueryResultCollection {
public:
   QueryResultCollection();
   ~QueryResultCollection();

   void addInsert(std::chrono::nanoseconds time, const std::string& tableName);
   void addCreate(std::chrono::nanoseconds time, const std::string& tableName);
   void addSelect(std::chrono::nanoseconds time, std::vector<std::vector<harriet::Value>>&& rows, const std::vector<std::string>& columnNames);

   void setParserError(uint32_t line, uint32_t column);
   void setRuntimeError(const std::string& message);

   bool hasError() const;
   const std::string& getErrorMessage() const;

   void print(std::ostream& os) const;

private:
   std::vector<std::unique_ptr<QueryResult>> singleResults;

   bool error;
   std::string errorMessage;
};

}
