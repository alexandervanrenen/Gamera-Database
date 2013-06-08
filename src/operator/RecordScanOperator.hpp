#pragma once

#include "common/Config.hpp"
#include "Operator.hpp"
#include "OperatorState.hpp"
#include "segment_manager/PageIdIterator.hpp"
#include <array>
#include <cstdint>
#include <memory>

namespace dbi {

class SPSegment;
class BufferManager;

/// Scan over a SPSegment with raw records
class RecordScanOperator : public Operator {
public:
   RecordScanOperator(SPSegment& input);
   virtual ~RecordScanOperator();

   virtual const Signature& getSignature() const;
   virtual void checkTypes() const throw(harriet::Exception);
   virtual void dump(std::ostream& os, uint32_t lvl) const;

   virtual void open();
   virtual bool next();
   virtual std::vector<std::unique_ptr<harriet::Value>> getOutput(); // Not supported .. not sure if this class is really an operator ..
   virtual const std::pair<TupleId, Record>& getRecord();
   virtual void close();

private:
   SPSegment& segment;
   PageIdIterator nextPage;
   std::vector<std::pair<TupleId, Record>> recordsInCurrentPage;
   uint32_t positionInCurrentPage;
   OperatorState state;
};

}
