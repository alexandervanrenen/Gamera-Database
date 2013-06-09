#pragma once

#include "Operator.hpp"
#include "OperatorState.hpp"
#include "signature/ProjectionSignature.hpp"
#include "schema/Common.hpp"
#include <array>
#include <cstdint>
#include <memory>

namespace dbi {

/// 
class ProjectionOperator : public Operator {
public:
   ProjectionOperator(std::unique_ptr<Operator> source, const std::vector<ColumnIdentifier>& projectedAttributes);
   virtual ~ProjectionOperator();

   virtual const Signature& getSignature() const;
   virtual void checkTypes() const throw(harriet::Exception);
   virtual void dump(std::ostream& os, uint32_t lvl) const;

   virtual void open();
   virtual bool next();
   virtual std::vector<std::unique_ptr<harriet::Value>> getOutput();
   virtual void close();

private:
   std::unique_ptr<Operator> source;
   OperatorState state;
   ProjectionSignature signature;
};

}
