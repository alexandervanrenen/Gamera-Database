#pragma once

#include "common/Config.hpp"
#include "Operator.hpp"
#include "OperatorState.hpp"
#include "segment_manager/PageIDIterator.hpp"
#include <array>
#include <cstdint>
#include <memory>

namespace dbi {

class SPSegment;
class BufferManager;

/// Scan over a SPSegment
class TableScanOperator : public Operator {
public:
   TableScanOperator(SPSegment& input);

   void open();

   bool next();

   const std::pair<TupleId, Record>& getOutput();

   void close();

   ~TableScanOperator();

private:
   SPSegment& segment;
   PageIDIterator nextPage;
   std::vector<std::pair<TupleId, Record>> recordsInCurrentPage;
   uint32_t positionInCurrentPage;
   OperatorState state;
};

}
