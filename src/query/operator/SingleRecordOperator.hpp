#pragma once

#include "Operator.hpp"
#include "OperatorState.hpp"
#include <memory>
#include <vector>

namespace harriet { class Value; }

namespace dbi {

class SPSegment;
class BufferManager;

namespace qopt { class GlobalRegister; }

/// Supplies one tuple, which was specified in the SQL script
/// Its used for a static input for the insert operator (e.g. insert into Students values(26120,"Fichte",10);)
class SingleRecordOperator : public Operator {
public:
   SingleRecordOperator(std::vector<harriet::Value>&& input, qopt::GlobalRegister& globalRegister);
   virtual ~SingleRecordOperator();

   virtual void dump(std::ostream& os, uint32_t lvl) const;

   virtual void open();
   virtual bool next();
   virtual void close();

private:
   OperatorState state;
   bool hasNext;
   std::vector<harriet::Value> tuple;

   qopt::GlobalRegister& globalRegister;
};

}
