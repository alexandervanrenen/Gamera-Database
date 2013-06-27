#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace harriet { class Value; }

namespace dbi {

class QueryResult;
class SelectResult;

class QueryResultCollection {
public:
   QueryResultCollection();
   ~QueryResultCollection();

   void addInsert(std::chrono::nanoseconds time, const std::string& tableName);
   void addCreate(std::chrono::nanoseconds time, const std::string& tableName);
   void addSelect(std::chrono::nanoseconds time, std::vector<std::vector<harriet::Value>>&& rows, const std::vector<std::string>& columnNames);
   void addPrintOutput(const std::string& accessTree);

   void setParserError(uint32_t line, uint32_t column);
   void setRuntimeError(const std::string& message);

   bool good() const;
   uint32_t size() const;
   std::vector<std::unique_ptr<QueryResult>>::iterator begin();
   std::vector<std::unique_ptr<QueryResult>>::iterator end();

   /// No errors, only one selection result => return this selection. Otherwise null.
   SelectResult* getAsSelect() const;

   const std::string& getErrorMessage() const;
   void print(std::ostream& os) const;
   void toJSON(std::ostream& os) const;

private:
   std::vector<std::unique_ptr<QueryResult>> singleResults;
   std::vector<std::string> accessTrees;

   bool error;
   std::string errorMessage;
};

}
