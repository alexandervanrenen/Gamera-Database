#include "DependencyAnalyser.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Environment.hpp"
#include "query/parser/Common.hpp"
#include "query/util/TableAccessInfo.hpp"
#include "query/util/ColumnAccessInfo.hpp"
#include "query/util/GlobalRegister.hpp"
#include "util/Utility.hpp"

using namespace std;

namespace dbi {

namespace qgen {

DependencyAnalyser::DependencyAnalyser(const harriet::Environment& environment, const std::vector<qopt::TableAccessInfo>& tableAccessInfos)
: environment(environment)
, tableAccessInfos(tableAccessInfos)
{
}

unique_ptr<qopt::GlobalRegister> DependencyAnalyser::createGlobalRegister(vector<pair<string, unique_ptr<harriet::Expression>>>& projections, vector<unique_ptr<harriet::Expression>>& predicates, vector<string>& orderBy)
{
   // Resolve all projections
   for(auto& projection : projections) {
      vector<string*> variableNames = projection.second->getAllVariableNames();
      for(auto variableName : variableNames)
         *variableName = resolveColumnReference(ColumnReference(*variableName));
   }

   // Resolve all predicates
   for(auto& predicate : predicates) {
      vector<string*> variableNames = predicate->getAllVariableNames();
      for(auto variableName : variableNames)
         *variableName = resolveColumnReference(ColumnReference(*variableName));
   }

   // Resolve all order columns
   for(auto& columnName : orderBy)
      columnName = resolveColumnReference(ColumnReference(columnName));
   
   // Construct global register
   return util::make_unique<qopt::GlobalRegister>(requiredColumns);
}

string DependencyAnalyser::resolveColumnReference(const ColumnReference& columnReference)
{
   // Try to find variable in the tables
   uint32_t tableId = -1;
   for(uint32_t id=0; id<tableAccessInfos.size(); id++) {
      // Check if variable is a column in this table
      if(columnReference.tableQualifier.size()==0 || columnReference.tableQualifier==tableAccessInfos[id].tableQualifier) {
         // Now we found the table
         if(tableAccessInfos[id].schema.hasColumn(columnReference.columnName)) {
            // Now we found the column => check if there was another one
            if(tableId == static_cast<uint32_t>(-1))
               tableId = id; else
               throw harriet::Exception{"ambiguous identifier '" + columnReference.tableQualifier + "." + columnReference.columnName + "', candidates: '" + tableAccessInfos[tableId].tableQualifier + "." + columnReference.columnName + "' or '" + tableAccessInfos[id].tableQualifier + "." + columnReference.columnName + "'"};
         }
      }
   }

   if(tableId != static_cast<uint32_t>(-1)) {
      // We found a column
      uint32_t columnIndex = tableAccessInfos[tableId].schema.getColumn(columnReference.columnName);
      const ColumnSchema& columnSchema = tableAccessInfos[tableId].schema.getAttributes()[columnIndex];
      ColumnReference column((columnReference.tableQualifier.size()==0)?tableAccessInfos[tableId].tableQualifier:columnReference.tableQualifier, columnReference.columnName);
      requiredColumns.insert(qopt::ColumnAccessInfo(column, columnSchema, tableId, columnIndex));
      return column.str();
   } else {
      // No column found => look in environment
      if(!environment.isInAnyScope(columnReference.str()))
         throw harriet::Exception{"unknown identifier: '" + columnReference.str() + "'"};
      return columnReference.str();
   }
}

}

}

