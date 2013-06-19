#include "ColumnResolver.hpp"
#include "TableAccessInfo.hpp"
#include "util/Utility.hpp"
#include "harriet/Environment.hpp"
#include "query_parser/Common.hpp"
#include <vector>

using namespace std;

namespace dbi {

namespace qopt {

ColumnResolver::ColumnResolver(const harriet::Environment& env)
: env(env)
{
}

ColumnResolver::Result ColumnResolver::resolveSelection(const string& name, const vector<TableAccessInfo>& tableAccessVec) const
{
   pair<uint32_t, const AttributeSchema*> result = tryFindColumn(name, tableAccessVec);
   if(result.first != static_cast<uint32_t>(-1)) {
      // We found a column
      return Result(util::make_unique<ColumnAccessInfo>(name, *result.second, result.first));
   } else {
      // No column found => look in environment
      if(!env.isInAnyScope(name))
         throw harriet::Exception{"unknown identifier: '" + name + "'"};
      return Result(nullptr);
   }
}

ColumnAccessInfo ColumnResolver::resolveProjection(const string& name, const vector<TableAccessInfo>& tableAccessVec) const
{
   // Check the tables if attribute not found we can not project on it
   pair<uint32_t, const AttributeSchema*> result = tryFindColumn(name, tableAccessVec);
   if(result.first != static_cast<uint32_t>(-1))
      throw harriet::Exception{"unknown identifier: '" + name + "'"};
   return ColumnAccessInfo(name, *result.second, result.first);
}

pair<uint32_t, const AttributeSchema*> ColumnResolver::tryFindColumn(const string& name, const vector<TableAccessInfo>& tableAccessVec)
{
   // Try to find variable in the tables
   ColumnReference ref(name);
   uint32_t tableId = -1;
   const AttributeSchema* columnSchema = nullptr;
   for(uint32_t id=0; id<tableAccessVec.size(); id++) {
      // Check if variable is a column in this table
      if(ref.tableQualifier.size()==0 || ref.tableQualifier==tableAccessVec[id].tableQualifier) {
         // Now we found the table
         columnSchema = tableAccessVec[id].schema.getAttribute(ref.columnName);
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
