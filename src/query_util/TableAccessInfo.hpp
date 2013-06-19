#pragma once

#include "schema/RelationSchema.hpp"
#include <string>
#include <vector>

namespace dbi {

class SPSegment;
class RelationSchema;

namespace qopt {

struct TableAccessInfo {
   TableAccessInfo(const RelationSchema& schema, SPSegment& segment, const std::string& tableQualifier) : schema(schema), segment(segment), tableQualifier(tableQualifier) {}
   const RelationSchema& schema;
   SPSegment& segment;
   const std::string tableQualifier; // The name given by the user or the actual table name
};

}

}

/// Define a hash for std::vector<TableAccessInfo*> -- Needed by the optimizer
namespace std {

template<>
struct hash<std::vector<dbi::qopt::TableAccessInfo*>> {
   size_t operator()(const std::vector<dbi::qopt::TableAccessInfo*>& input) const {
      size_t result = 0;
      for(auto iter : input)
         result ^= iter->schema.getSegmentId().toInteger();
      return result;
   }
};

}
