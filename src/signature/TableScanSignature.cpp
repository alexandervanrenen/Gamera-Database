#include "TableScanSignature.hpp"
#include "schema/RelationSchema.hpp"
#include "query_util/TableAccessInfo.hpp"
#include "query_util/ColumnAccessInfo.hpp"
#include <iostream>

using namespace std;

namespace dbi {

TableScanSignature::TableScanSignature(const qopt::TableAccessInfo& tableAccessInfo, const set<qopt::ColumnAccessInfo>& requiredColumns, uint32_t registerOffset)
: tableAccessInfo(tableAccessInfo)
{
   // Gather available columns (transform the AttributeSchema into a AttributeSignature)
   vector<AttributeSignature> availableColumns;
   for(uint32_t i=0; i<tableAccessInfo.schema.getAttributes().size(); i++) {
      auto& attribute = tableAccessInfo.schema.getAttributes()[i];
      availableColumns.push_back(AttributeSignature{attribute.name, tableAccessInfo.tableQualifier, attribute.notNull, attribute.primaryKey, attribute.type, i, tableAccessInfo.tableId});
   }

   // Create mapping TableTupleIndex -> GlobalRegisterIndex (columnIndexes[2]=5 means that the column in the tables tuple with index 5 is loaded into global register at index 2)
   uint32_t index = 0;
   for(auto& req : requiredColumns) {
      // Not all required attributes of the query are related to this table
      if(req.tableIndex == tableAccessInfo.tableId) {
         auto attribute = getAttribute(availableColumns, req.tableIndex, req.attributeSchema.name);
         attributes.push_back(attribute);
         columnMapping.push_back(attributes.back().index);
         attributes.back().index = (index++) + registerOffset;
      }
   }
}

void TableScanSignature::dump(ostream& os) const
{
   Signature::dump(os);
   os << " from [";
   for(auto iter : columnMapping)
      os << " " << iter;
   os << " ]";
}

const vector<uint32_t>& TableScanSignature::getMapping() const
{
   return columnMapping;
}

}
