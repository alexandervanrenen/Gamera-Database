#pragma once

#include "Operator.hpp"
#include "OperatorState.hpp"
#include "query/signature/ZeroRecordSignature.hpp"
#include <memory>
#include <set>

namespace dbi {

namespace qopt { class ColumnAccessInfo; }

/// Supplies no tuples.
/// Its used for a select statement containing a false predicate which results in no tuples.
class ZeroRecordOperator : public Operator {
public:
   ZeroRecordOperator(std::set<qopt::ColumnAccessInfo>& projections);
   virtual ~ZeroRecordOperator();

   virtual const Signature& getSignature() const;
   virtual void dump(std::ostream& os, uint32_t lvl) const;

   virtual void open();
   virtual bool next();
   virtual void close();

private:
   OperatorState state;
   const ZeroRecordSignature signature;
};

}
