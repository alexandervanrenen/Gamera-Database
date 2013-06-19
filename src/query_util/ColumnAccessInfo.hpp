#pragma once

#include "schema/RelationSchema.hpp"

namespace dbi {

namespace qopt {

struct ColumnAccessInfo {
   ColumnAccessInfo(std::string scriptName, const AttributeSchema& attributeSchema, uint32_t tableIndex) : scriptName(scriptName), attributeSchema(attributeSchema), tableIndex(tableIndex) {}
   const std::string scriptName; // The name of the variable in the script
   const AttributeSchema& attributeSchema;
   const uint32_t tableIndex; // The index in the table vector (generated from the FROM clause)
};

}

}
