#pragma once

#include "schema/RelationSchema.hpp"
#include "query/parser/Common.hpp"
#include <ios>

namespace harriet { class Variable; class Expression; }

namespace dbi {

class ColumnReference;

namespace qopt {

struct ColumnAccessInfo {
   /// Constructor
   ColumnAccessInfo(const ColumnReference& column, const ColumnSchema& columnSchema, uint32_t tableIndex, uint32_t columnIndex);

   /// The column reference in the script
   const ColumnReference columnReference;

   /// The schema of this column
   const ColumnSchema& columnSchema;
   const uint32_t tableIndex; // The index in the table vector (generated from the FROM clause)
   const uint32_t columnIndex; // The index in the tables tuple.

   void dump(std::ostream& os) const;

   /// Check if two ColumnAccessInfos refer to the same column
   friend bool operator==(const ColumnAccessInfo& lhs, const ColumnAccessInfo& rhs);
   friend bool operator<(const ColumnAccessInfo& lhs, const ColumnAccessInfo& rhs);
};

}

}
