#pragma once

#include "query/parser/Common.hpp"
#include "Signature.hpp"
#include <set>

namespace harriet { class Value; }

namespace dbi {

namespace qopt { class ColumnAccessInfo; class TableAccessInfo; }

class RelationSchema;

class TableScanSignature : public Signature {
public:
   // Create named variables from a TableScanOperator
   TableScanSignature(const qopt::TableAccessInfo& tableAccessInfo, const std::set<qopt::ColumnAccessInfo>& requiredColumns, uint32_t registerOffset);

   virtual void dump(std::ostream& os) const;

   void prepare(const std::set<qopt::ColumnAccessInfo>& requiredColumns, uint32_t registerOffset);

   const std::vector<uint32_t>& getMapping() const;

private:
   const qopt::TableAccessInfo& tableAccessInfo;
   std::vector<uint32_t> columnMapping; // Maps TableTupleIndex -> GlobalRegisterIndex (columnIndexes[2]=5 means that the column in the tables tuple with index 5 is loaded into global register at index 2)
};

}
