#pragma once

#include "Operator.hpp"
#include "OperatorState.hpp"
#include "signature/ProjectionSignature.hpp"
#include <array>
#include <cstdint>
#include <memory>

namespace dbi {

namespace qopt { class ColumnAccessInfo; }

/// 
class ProjectionOperator : public Operator {
public:
   ProjectionOperator(std::unique_ptr<Operator> source, const std::vector<qopt::ColumnAccessInfo>& projectedAttributes, std::vector<harriet::Value>& globalRegister);
   virtual ~ProjectionOperator();

   virtual const Signature& getSignature() const;
   virtual void prepare(std::vector<harriet::Value>& globalRegister, const std::set<qopt::ColumnAccessInfo>& requiredColumns);
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
