#pragma once

#include "common/Config.hpp"
#include "Operator.hpp"
#include "OperatorState.hpp"
#include "segment_manager/PageIdIterator.hpp"
#include "schema/RelationSchema.hpp"
#include <array>
#include <cstdint>
#include <memory>

namespace dbi {

class SPSegment;
class BufferManager;
class RecordScanOperator;

/// Scan over a SPSegment
class TableScanOperator : public Operator {
public:
   TableScanOperator(std::unique_ptr<RecordScanOperator> scanner, const RelationSchema& schema);
   virtual ~TableScanOperator();

   virtual const RelationSchema& getSignature() const;

   virtual void open();
   virtual bool next();
   virtual std::vector<std::unique_ptr<harriet::Value>> getOutput();
   virtual void close();

private:
   std::unique_ptr<RecordScanOperator> scanner;
   OperatorState state;
   const RelationSchema schema;
};

}
