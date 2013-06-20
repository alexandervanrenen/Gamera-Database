#pragma once

#include "harriet/Expression.hpp"
#include "schema/RelationSchema.hpp"
#include <cassert>

namespace harriet { class Variable; class Expression; }

namespace dbi {

namespace qopt {

// TODO: use cpp file for implementation you idiot
struct ColumnAccessInfo {
   /// Constructor
   ColumnAccessInfo(std::unique_ptr<harriet::Expression>& variable, const ColumnSchema& columnSchema, uint32_t tableIndex) : variable(variable), columnSchema(columnSchema), tableIndex(tableIndex) {assert(variable->getExpressionType() == harriet::ExpressionType::TVariable);}

   /// The column reference in the script
   std::unique_ptr<harriet::Expression>& variable;
   harriet::Variable& getVariable() {assert(variable->getExpressionType() == harriet::ExpressionType::TVariable); return reinterpret_cast<harriet::Variable&>(*variable);}

   /// The schema of this column
   const ColumnSchema& columnSchema; // TODO: change: use only type and name
   const uint32_t tableIndex; // The index in the table vector (generated from the FROM clause)

   /// Check if two ColumnAccessInfos refer to the same column
   friend bool operator==(const ColumnAccessInfo& lhs, const ColumnAccessInfo& rhs) {return lhs.tableIndex==rhs.tableIndex && lhs.columnSchema.name==rhs.columnSchema.name;}
   friend bool operator<(const ColumnAccessInfo& lhs, const ColumnAccessInfo& rhs) {if(lhs.tableIndex==rhs.tableIndex) return lhs.columnSchema.name<rhs.columnSchema.name; else return lhs.tableIndex<rhs.tableIndex; }
};

}

}
