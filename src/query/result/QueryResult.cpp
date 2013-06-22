#include "QueryResult.hpp"
#include "harriet/Value.hpp"
#include "util/Utility.hpp"
#include <cassert>
#include <iomanip>
#include <iostream>

using namespace std;

namespace dbi {

QueryResult::~QueryResult()
{
}

SelectResult::SelectResult(chrono::nanoseconds nanos, vector<vector<harriet::Value> >&& result, const vector<string>& columnNames)
: nanos(nanos)
, result(move(result))
, columnNames(columnNames)
{
   assert(result.empty()?true:result[0].size()==columnNames.size());
}

SelectResult::~SelectResult()
{
}

void SelectResult::print(ostream& os) const
{
   // Calculate column widths
   uint32_t totalWidth = 1;
   vector<uint32_t> columnWidths;
   for(uint32_t i=0; i<columnNames.size(); i++) {
      columnWidths.push_back(max((uint32_t)columnNames[i].size(), result.empty()?0:result[0][i].type.getMaxValuesASCIIRepresentationSize()));
      totalWidth += columnWidths.back() + 3;
   }

   // Print header
   os << setfill(' ') << left << string(totalWidth, '-') << endl << "| ";
   for(uint32_t i=0; i<columnNames.size(); i++)
      os << setw(columnWidths[i]) << columnNames[i] << " | ";
   os << endl << string(totalWidth, '-') << endl;

   // Print content
   uint64_t tupleCount = 0;
   for(auto& row : result) {
      os << "| ";
      for(uint32_t i=0; i<row.size(); i++)
         os << setw(columnWidths[i]) << row[i] << " | ";
      os << endl;
      tupleCount++;
   }
   os << string(totalWidth, '-') << endl;
   os << "Fetched " << tupleCount << " tuples in " << util::formatTime(nanos, 3) << endl;
}

CreateResult::CreateResult(chrono::nanoseconds nanos, const string& tableName)
: nanos(nanos)
, tableName(tableName)
{
}

CreateResult::~CreateResult()
{
}

void CreateResult::print(ostream& os) const
{
   os << "Success: Created table '" << tableName << "' in " << util::formatTime(nanos, 3) << endl;
}

InsertResult::InsertResult(chrono::nanoseconds nanos, const string& tableName)
: nanos(nanos)
, tableName(tableName)
{
}

InsertResult::~InsertResult()
{
}

void InsertResult::print(ostream& os) const
{
   os << "Success: Insert into '" << tableName << "' in " << util::formatTime(nanos, 3) << endl;
}

}
