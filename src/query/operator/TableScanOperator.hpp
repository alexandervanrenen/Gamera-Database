#pragma once

#include "Operator.hpp"
#include "OperatorState.hpp"
#include "query/signature/TableScanSignature.hpp"
#include "segment_manager/PageIdIterator.hpp"
#include <cstdint>
#include <memory>

namespace dbi {

namespace qopt { class TableAccessInfo; }

class SPSegment;
class Record;

/// Interprets the records provided by a RecordScanOperator
class TableScanOperator : public Operator {
public:
   TableScanOperator(const qopt::TableAccessInfo& tableaccessInfo, const std::set<qopt::ColumnAccessInfo>& requiredColumns, std::vector<harriet::Value>& globalRegister, uint32_t& registerPosition);
   virtual ~TableScanOperator();

   virtual const Signature& getSignature() const;
   virtual void dump(std::ostream& os, uint32_t lvl) const;

   virtual void open();
   virtual bool next();
   virtual void close();

private:
   const qopt::TableAccessInfo& tableaccessInfo;
   TableScanSignature signature;

   OperatorState state;
   PageIdIterator nextPage;
   std::vector<std::pair<TupleId, Record>> recordsInCurrentPage;
   uint32_t positionInCurrentPage;

   std::vector<harriet::Value>& globalRegister;
   uint32_t registerOffset;
};

}
