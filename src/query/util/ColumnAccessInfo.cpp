#include "ColumnAccessInfo.hpp"

using namespace std;

namespace dbi {

namespace qopt {

ColumnAccessInfo::ColumnAccessInfo(const ColumnReference& column, const ColumnSchema& columnSchema, uint32_t tableIndex, uint32_t columnIndex)
: columnReference(column)
, columnSchema(columnSchema)
, tableIndex(tableIndex)
, columnIndex(columnIndex)
{ 
}

void ColumnAccessInfo::dump(ostream& os) const
{
   os << columnSchema.type << " " << columnReference.str() << " " << tableIndex << " " << columnReference.str();
}

bool operator==(const ColumnAccessInfo& lhs, const ColumnAccessInfo& rhs)
{
  return lhs.tableIndex==rhs.tableIndex && lhs.columnSchema.name==rhs.columnSchema.name;
}

bool operator<(const ColumnAccessInfo& lhs, const ColumnAccessInfo& rhs)
{
  if(lhs.tableIndex==rhs.tableIndex)
    return lhs.columnSchema.name<rhs.columnSchema.name; else
    return lhs.tableIndex<rhs.tableIndex;
}

}

}

