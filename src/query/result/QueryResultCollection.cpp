#include "QueryResultCollection.hpp"
#include "QueryResult.hpp"
#include "util/Utility.hpp"
#include <cassert>
#include <iostream>

using namespace std;

namespace dbi {

QueryResultCollection::QueryResultCollection()
: error(false)
{
}

QueryResultCollection::~QueryResultCollection()
{
}

void QueryResultCollection::addInsert(chrono::nanoseconds time, const string& tableName)
{
   assert(!error);
   singleResults.push_back(util::make_unique<InsertResult>(time, tableName));
}

void QueryResultCollection::addCreate(chrono::nanoseconds time, const string& tableName)
{
   assert(!error);
   singleResults.push_back(util::make_unique<CreateResult>(time, tableName));
}

void QueryResultCollection::addSelect(chrono::nanoseconds time, vector<vector<harriet::Value>>&& rows, const vector<string>& columnNames)
{
   assert(!error);
   singleResults.push_back(util::make_unique<SelectResult>(time, move(rows), columnNames));
}

void QueryResultCollection::addPrintOutput(const string& accessTree)
{
   accessTrees.push_back(accessTree);
}

void QueryResultCollection::setParserError(uint32_t line, uint32_t column)
{
   assert(!error);
   error = true;
   errorMessage = "A parser error occurred: line " + to_string(line) + " column " + to_string(column);
}

void QueryResultCollection::setRuntimeError(const string& message)
{
   assert(!error);
   error = true;
   errorMessage = "A runtime error: " + message;
}

bool QueryResultCollection::good() const
{
   return !error;
}

uint32_t QueryResultCollection::size() const
{
   return singleResults.size();
}

vector<unique_ptr<QueryResult>>::iterator QueryResultCollection::begin()
{
   return singleResults.begin();
}

vector<unique_ptr<QueryResult>>::iterator QueryResultCollection::end()
{
   return singleResults.end();
}

SelectResult* QueryResultCollection::getAsSelect() const
{
   if(error || singleResults.size()!=1 || singleResults[0]->getType()!=QueryType::kSelect)
      return nullptr; else
      return reinterpret_cast<SelectResult*>(singleResults[0].get());
}

const string& QueryResultCollection::getErrorMessage() const
{
   assert(error);
   return errorMessage;
}

void QueryResultCollection::print(ostream& os) const
{
   for(auto& iter : accessTrees)
      os << iter << endl;
   for(auto& iter : singleResults)
      iter->print(os);
   if(error)
      os << errorMessage << endl;
}

void QueryResultCollection::toJSON(ostream& os) const
{
   // Begin {
   os << "{";

   // Print error (message is empty if no error occured)
   os << "\"error\":" << "\"" << errorMessage << "\"," << endl;

   // Print all results
   os << "\"results\":" << "[" << endl;
   for(uint32_t i=1; i<singleResults.size(); i++) {
      singleResults[i-1]->toJSON(os);
      os << "," << endl;
   }
   if(!singleResults.empty())
      singleResults.back()->toJSON(os);
   os << endl << "]" << endl;

   // Finish }
   os << "}" << endl;
}

}
