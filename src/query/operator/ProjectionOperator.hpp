#pragma once

#include "Operator.hpp"
#include "OperatorState.hpp"
#include "query/signature/ProjectionSignature.hpp"
#include <cstdint>
#include <memory>

namespace dbi {

namespace qopt { class ColumnAccessInfo; }

/// This operator changes only the signature. It is able to reorder the supplied columns and drop them.
class ProjectionOperator : public Operator {
public:
   ProjectionOperator(std::unique_ptr<Operator> source, const std::vector<qopt::ColumnAccessInfo>& projectedAttributes);
   virtual ~ProjectionOperator();

   virtual const Signature& getSignature() const;
   virtual void dump(std::ostream& os, uint32_t lvl) const;

   virtual void open();
   virtual bool next();
   virtual void close();

private:
   std::unique_ptr<Operator> source;
   OperatorState state;
   ProjectionSignature signature;
};

}
