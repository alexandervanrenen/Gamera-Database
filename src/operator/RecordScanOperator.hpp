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

/// Scan over a SPSegment
class RecordScanOperator : public Operator {
public:
   RecordScanOperator(SPSegment& input);
   ~RecordScanOperator();

   virtual const RelationSchema& getSignatur() const;

   void open();
   bool next();
   const std::vector<std::unique_ptr<harriet::Value>> getOutput(); // Not supported .. not sure if this class is really an operator ..
   const std::pair<TupleId, Record>& getRecord();
   void close();

private:
   SPSegment& segment;
   PageIdIterator nextPage;
   std::vector<std::pair<TupleId, Record>> recordsInCurrentPage;
   uint32_t positionInCurrentPage;
   OperatorState state;
};

}
