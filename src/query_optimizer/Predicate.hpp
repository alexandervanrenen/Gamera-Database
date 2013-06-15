#pragma once

#include "schema/RelationSchema.hpp"
#include "TableAccessInfo.hpp"
#include <memory>
#include <vector>
#include <ios>
#include <set>

namespace harriet { class Expression; }

namespace dbi {

namespace qopt {

struct ColumnAccessInfo {
   ColumnAccessInfo(std::string scriptName, const AttributeSchema& attributeSchema) : scriptName(scriptName), attributeSchema(attributeSchema) {}
   std::string scriptName; // The name of the variable in the script
   const AttributeSchema& attributeSchema;
};

struct Predicate {
   std::vector<ColumnAccessInfo> columns;
   std::set<uint32_t> tables;
   std::unique_ptr<harriet::Expression> condition;

   void dump(std::ostream& os) const;
};

}

}
