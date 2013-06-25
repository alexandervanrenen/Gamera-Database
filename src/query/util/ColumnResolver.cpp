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

ColumnResolver::Result ColumnResolver::resolveColumnReference(const ColumnReference& columnReference, const vector<TableAccessInfo>& tableAccessVec) const
{
   // Try to find variable in the tables
   uint32_t tableId = -1;
   for(uint32_t id=0; id<tableAccessVec.size(); id++) {
      // Check if variable is a column in this table
      if(columnReference.tableQualifier.size()==0 || columnReference.tableQualifier==tableAccessVec[id].tableQualifier) {
         // Now we found the table
         if(tableAccessVec[id].schema.hasColumn(columnReference.columnName)) {
            // Now we found the column => check if there was another one
            if(tableId == static_cast<uint32_t>(-1))
               tableId = id; else
               throw harriet::Exception{"ambiguous identifier '" + columnReference.tableQualifier + "." + columnReference.columnName + "', candidates: '" + tableAccessVec[tableId].tableQualifier + "." + columnReference.columnName + "' or '" + tableAccessVec[id].tableQualifier + "." + columnReference.columnName + "'"};
         }
      }
   }

   if(tableId != static_cast<uint32_t>(-1)) {
      // We found a column
      uint32_t columnIndex = tableAccessVec[tableId].schema.getColumn(columnReference.columnName);
      const ColumnSchema& columnSchema = tableAccessVec[tableId].schema.getAttributes()[columnIndex];
      ColumnReference column((columnReference.tableQualifier.size()==0)?tableAccessVec[tableId].schema.getName():columnReference.tableQualifier, columnReference.columnName);
      return Result(util::make_unique<ColumnAccessInfo>(column, columnSchema, tableId, columnIndex));
   } else {
      // No column found => look in environment
      if(!env.isInAnyScope(columnReference.str()))
         throw harriet::Exception{"unknown identifier: '" + columnReference.str() + "'"};
      return Result(nullptr);
   }
}

}

}
