#pragma once

#include "Operator.hpp"
#include "OperatorState.hpp"
#include "schema/Signature.hpp"
#include <array>
#include <cstdint>
#include <memory>

namespace dbi {

class SPSegment;
class BufferManager;
class RecordScanOperator;

/// Interprets the records provided by a RecordScanOperator
class TableScanOperator : public Operator {
public:
   TableScanOperator(std::unique_ptr<RecordScanOperator> scanner, const RelationSchema& schema, const std::string& alias);
   virtual ~TableScanOperator();

   virtual const Signature& getSignature() const;
   virtual void checkTypes() const throw(harriet::Exception);
   virtual void dump(std::ostream& os, uint32_t lvl) const;

   virtual void open();
   virtual bool next();
   virtual std::vector<std::unique_ptr<harriet::Value>> getOutput();
   virtual void close();

private:
   std::unique_ptr<RecordScanOperator> scanner;
   OperatorState state;
   const RelationSchema& schema;
   const Signature signature;
};

}
