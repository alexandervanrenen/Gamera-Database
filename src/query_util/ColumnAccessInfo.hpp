#pragma once

#include "schema/RelationSchema.hpp"

namespace dbi {

namespace qopt {

struct ColumnAccessInfo {
   ColumnAccessInfo(std::string scriptName, const ColumnSchema& columnSchema, uint32_t tableIndex) : scriptName(scriptName), columnSchema(columnSchema), tableIndex(tableIndex) {}
   const std::string scriptName; // The name of the variable in the script
   const ColumnSchema& columnSchema;
   const uint32_t tableIndex; // The index in the table vector (generated from the FROM clause)
};

}

}
