#pragma once

#include "query/parser/Common.hpp"
#include <set>
#include <vector>

namespace harriet { class Value; }

namespace dbi {

class Record;

namespace qopt { class TableAccessInfo; class GlobalRegister; }

class RelationSchema;

class TableScanSignature {
public:
   // Create named variables from a TableScanOperator
   TableScanSignature(const qopt::TableAccessInfo& tableAccessInfo, qopt::GlobalRegister& globalRegister);

   void loadRecordIntoGlobalRegister(Record& record) const;

   virtual void dump(std::ostream& os) const;

private:
   const qopt::TableAccessInfo& tableAccessInfo;
   qopt::GlobalRegister& globalRegister;
   struct Mapping {
      uint32_t tupleIndex; // Index in the tuple loaded from the table
      uint32_t registerIndex; // Index in the global register
   };
   std::vector<Mapping> columnMapping;
};

}
