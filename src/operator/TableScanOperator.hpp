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

   const Record& getOutput();

   void close();

   ~TableScanOperator();

private:
   SPSegment& segment;
   PageIDIterator nextPage;
   std::vector<Record> recordsInCurrentPage;
   uint32_t positionInCurrentPage;
   OperatorState state;
};

}
