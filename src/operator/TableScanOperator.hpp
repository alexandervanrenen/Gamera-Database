#pragma once

#include "Operator.hpp"
#include "OperatorState.hpp"
#include "signature/TableScanSignature.hpp"
#include "segment_manager/PageIdIterator.hpp"
#include <array>
#include <cstdint>
#include <memory>

namespace dbi {

class SPSegment;

/// Interprets the records provided by a RecordScanOperator
class TableScanOperator : public Operator {
public:
   TableScanOperator(SPSegment& source, const RelationSchema& underlyingSchema, const std::string& alias);
   virtual ~TableScanOperator();

   virtual const Signature& getSignature() const;
   virtual void checkTypes() const throw(harriet::Exception);
   virtual void dump(std::ostream& os, uint32_t lvl) const;

   virtual void open();
   virtual bool next();
   virtual std::vector<std::unique_ptr<harriet::Value>> getOutput();
   virtual void close();

private:
   SPSegment& source;
   const RelationSchema& underlyingSchema;
   const TableScanSignature signature;

   OperatorState state;
   PageIdIterator nextPage;
   std::vector<std::pair<TupleId, Record>> recordsInCurrentPage;
   uint32_t positionInCurrentPage;
};

}
