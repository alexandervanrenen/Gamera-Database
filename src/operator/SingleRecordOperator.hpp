#pragma once

#include "common/Config.hpp"
#include "Operator.hpp"
#include "OperatorState.hpp"
#include "segment_manager/PageIdIterator.hpp"
#include "schema/Signature.hpp"
#include <array>
#include <cstdint>
#include <memory>

namespace dbi {

class SPSegment;
class BufferManager;
class RecordScanOperator;

/// Supplies one tuple, which was specified in the SQL script
/// Its used for a static input for the insert operator (e.g. insert into Students values(26120,"Fichte",10);)
class SingleRecordOperator : public Operator {
public:
   SingleRecordOperator(const std::vector<std::unique_ptr<harriet::Value>>& input);
   virtual ~SingleRecordOperator();

   virtual const Signature& getSignature() const;
   virtual void checkTypes() const throw(harriet::Exception);
   virtual void dump(std::ostream& os, uint32_t lvl) const;

   virtual void open();
   virtual bool next();
   virtual std::vector<std::unique_ptr<harriet::Value>> getOutput();
   virtual void close();

private:
   std::vector<std::unique_ptr<harriet::Value>> values;
   OperatorState state;
   bool hasNext;
   const Signature signature;
};

}
