#pragma once

#include "common/Config.hpp"
#include "Operator.hpp"
#include "segment_manager/PageIDIterator.hpp"
#include "OperatorState.hpp"
#include <cstdint>
#include <array>
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

   const std::pair<TId, Record>& getOutput();

   void close();

   ~TableScanOperator();

private:
   SPSegment& segment;
   PageIDIterator nextPage;
   std::vector<std::pair<TId, Record>> recordsInCurrentPage;
   uint32_t positionInCurrentPage;
   OperatorState state;
};

}
