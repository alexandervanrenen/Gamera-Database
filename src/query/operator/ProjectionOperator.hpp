#pragma once

#include "Operator.hpp"
#include "OperatorState.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace harriet { class VariableType; }

namespace dbi {

namespace qopt { class Projection; class GlobalRegister; }

/// This operator changes only the signature. It is able to reorder the supplied columns and drop them.
class ProjectionOperator : public Operator {
public:
   ProjectionOperator(std::unique_ptr<Operator> source, std::vector<std::unique_ptr<qopt::Projection>>&& projections, qopt::GlobalRegister& globalRegister);
   virtual ~ProjectionOperator();

   std::vector<uint32_t> getRegisterIndexes() const;
   virtual void dump(std::ostream& os, uint32_t lvl) const;

   virtual void open();
   virtual bool next();
   virtual void close();

private:
   std::unique_ptr<Operator> source;
   OperatorState state;
   std::vector<std::unique_ptr<qopt::Projection>> projections;
   qopt::GlobalRegister& globalRegister;
};

}
