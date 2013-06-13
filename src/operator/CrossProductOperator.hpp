#pragma once

#include "Operator.hpp"
#include "OperatorState.hpp"
#include "signature/CrossProductSignature.hpp"
#include "schema/Common.hpp"
#include <cstdint>
#include <memory>

namespace dbi {

/// 
class CrossProductOperator : public Operator {
public:
   CrossProductOperator(std::unique_ptr<Operator> lhs, std::unique_ptr<Operator> rhs);
   virtual ~CrossProductOperator();

   virtual const Signature& getSignature() const;
   virtual void checkTypes() const throw(harriet::Exception);
   virtual void dump(std::ostream& os, uint32_t lvl) const;

   virtual void open();
   virtual bool next();
   virtual std::vector<harriet::Value> getOutput();
   virtual void close();

private:
   std::unique_ptr<Operator> lhs;
   std::unique_ptr<Operator> rhs;
   bool lhsHasNext;
   OperatorState state;
   CrossProductSignature signature;
};

}
