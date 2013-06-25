#pragma once

#include "Operator.hpp"
#include "OperatorState.hpp"
#include "query/signature/SelectionSignature.hpp"
#include <memory>
#include <vector>

namespace dbi {

namespace qopt { class Predicate; class GlobalRegister; }

/// Applies a predicate to the incomming tuples.
class SelectionOperator : public Operator {
public:
   SelectionOperator(std::unique_ptr<Operator> source, std::unique_ptr<qopt::Predicate> predicate, qopt::GlobalRegister& globalRegister);
   virtual ~SelectionOperator();

   virtual void dump(std::ostream& os, uint32_t lvl) const;

   virtual void open();
   virtual bool next();
   virtual void close();

private:
   std::unique_ptr<Operator> source;
   OperatorState state;
   SelectionSignature signature;
};

}
