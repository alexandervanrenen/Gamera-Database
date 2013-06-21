#pragma once

#include "QueryType.hpp"
#include <string>
#include <vector>
#include <chrono>

namespace harriet { class Value; }

namespace dbi {

struct QueryResult {
   virtual ~QueryResult();
   virtual QueryType getType() const = 0;
   virtual void print(std::ostream& os) const = 0;
};

struct SelectResult : public QueryResult {
   SelectResult(std::chrono::nanoseconds nanos, std::vector<std::vector<harriet::Value>>&& result, const std::vector<std::string>& columnNames);
   virtual ~SelectResult();

   std::chrono::nanoseconds nanos;
   std::vector<std::vector<harriet::Value>> result;
   std::vector<std::string> columnNames;

   virtual QueryType getType() const { return QueryType::kSelect; };
   virtual void print(std::ostream& os) const;
};

struct CreateResult : public QueryResult {
   CreateResult(std::chrono::nanoseconds nanos, const std::string& tableName);
   virtual ~CreateResult();

   std::chrono::nanoseconds nanos;
   const std::string tableName;

   virtual QueryType getType() const { return QueryType::kCreate; };
   virtual void print(std::ostream& os) const;
};

struct InsertResult : public QueryResult {
   InsertResult(std::chrono::nanoseconds nanos, const std::string& tableName);
   virtual ~InsertResult();

   std::chrono::nanoseconds nanos;
   const std::string tableName;

   virtual QueryType getType() const { return QueryType::kInsert; };
   virtual void print(std::ostream& os) const;
};

}
