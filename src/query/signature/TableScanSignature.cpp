#include "TableScanSignature.hpp"
#include "query/util/ColumnAccessInfo.hpp"
#include "query/util/TableAccessInfo.hpp"
#include "schema/RelationSchema.hpp"
#include "query/util/GlobalRegister.hpp"
#include <iostream>

using namespace std;

namespace dbi {

TableScanSignature::TableScanSignature(const qopt::TableAccessInfo& tableAccessInfo, qopt::GlobalRegister& globalRegister)
: tableAccessInfo(tableAccessInfo)
, globalRegister(globalRegister)
{
   auto requiredColumnIndexes = globalRegister.getColumnIndexes(tableAccessInfo.tableId);
   for(auto iter : requiredColumnIndexes)
      columnMapping.push_back(Mapping{globalRegister.getType(iter).columnIndex, iter});
}

void TableScanSignature::loadRecordIntoGlobalRegister(Record& record) const
{
   for(auto& iter : columnMapping)
      tableAccessInfo.schema.loadTuple(record, globalRegister.getValue(iter.registerIndex), iter.tupleIndex);
}

void TableScanSignature::dump(ostream& os) const
{
   os << " from [";
   for(auto iter : columnMapping)
      os << " r" << iter.registerIndex << " t" << iter.tupleIndex;
   os << " ]";
}

}
