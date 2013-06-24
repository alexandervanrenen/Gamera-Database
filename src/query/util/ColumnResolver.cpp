#include "ColumnResolver.hpp"
#include "harriet/Environment.hpp"
#include "query/parser/Common.hpp"
#include "TableAccessInfo.hpp"
#include "util/Utility.hpp"
#include <vector>

using namespace std;

namespace dbi {

namespace qopt {

ColumnResolver::ColumnResolver(const harriet::Environment& env)
: env(env)
{
}

ColumnResolver::Result ColumnResolver::resolveSelection(const ColumnReference& column, const vector<TableAccessInfo>& tableAccessVec) const
{
   pair<uint32_t, const ColumnSchema*> result = tryFindColumn(column, tableAccessVec);
   if(result.first != static_cast<uint32_t>(-1)) {
      // We found a column
      return Result(util::make_unique<ColumnAccessInfo>(column, *result.second, result.first));
   } else {
      // No column found => look in environment
      if(!env.isInAnyScope(column.str()))
         throw harriet::Exception{"unknown identifier: '" + column.str() + "'"};
      return Result(nullptr);
   }
}

ColumnAccessInfo ColumnResolver::resolveProjection(const ColumnReference& column, const vector<TableAccessInfo>& tableAccessVec) const
{
   // Check the tables if attribute not found we can not project on it
   pair<uint32_t, const ColumnSchema*> result = tryFindColumn(column, tableAccessVec);
   if(result.first == static_cast<uint32_t>(-1))
      throw harriet::Exception{"unknown identifier: '" + column.tableQualifier + "." + column.columnName + "'"};
   return ColumnAccessInfo(column, *result.second, result.first);
}

pair<uint32_t, const ColumnSchema*> ColumnResolver::tryFindColumn(const ColumnReference& ref, const vector<TableAccessInfo>& tableAccessVec)
{
   // Try to find variable in the tables
   uint32_t tableId = -1;
   const ColumnSchema* columnSchema = nullptr;
   for(uint32_t id=0; id<tableAccessVec.size(); id++) {
      // Check if variable is a column in this table
      if(ref.tableQualifier.size()==0 || ref.tableQualifier==tableAccessVec[id].tableQualifier) {
         // Now we found the table
         columnSchema = tableAccessVec[id].schema.getColumn(ref.columnName);
         if(columnSchema != nullptr) {
            // Now we found the column => check if there was another one
            if(tableId == static_cast<uint32_t>(-1))
               tableId = id; else
               throw harriet::Exception{"ambiguous identifier '" + ref.tableQualifier + "." + ref.columnName + "', candidates: '" + tableAccessVec[tableId].tableQualifier + "." + ref.columnName + "' or '" + tableAccessVec[id].tableQualifier + "." + ref.columnName + "'"};
         }
      }
   }
   return make_pair(tableId, columnSchema);
}

}

}
