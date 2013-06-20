#pragma once

#include "Operator.hpp"
#include "OperatorState.hpp"
#include "signature/SingleRecordSignature.hpp"
#include <memory>

namespace dbi {

class SPSegment;
class BufferManager;

/// Supplies one tuple, which was specified in the SQL script
/// Its used for a static input for the insert operator (e.g. insert into Students values(26120,"Fichte",10);)
class SingleRecordOperator : public Operator {
public:
   SingleRecordOperator(std::vector<harriet::Value>&& input, std::vector<harriet::Value>& globalRegister);
   virtual ~SingleRecordOperator();

   virtual const Signature& getSignature() const;
   virtual void prepare(std::vector<harriet::Value>& globalRegister, const std::set<qopt::ColumnAccessInfo>& requiredColumns);
   virtual void dump(std::ostream& os, uint32_t lvl) const;

   virtual void open();
   virtual bool next();
   virtual void close();

private:
   OperatorState state;
   bool hasNext;
   const SingleRecordSignature signature;
   std::vector<harriet::Value> tuple;

   std::vector<harriet::Value>& globalRegister;
   uint32_t registerOffset;
};

}
