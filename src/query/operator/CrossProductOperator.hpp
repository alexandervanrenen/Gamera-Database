#pragma once

#include "Operator.hpp"
#include "OperatorState.hpp"
#include <cstdint>
#include <memory>

namespace dbi {

/// Produces the cross product (not nested)
class CrossProductOperator : public Operator {
public:
   CrossProductOperator(std::unique_ptr<Operator> lhs, std::unique_ptr<Operator> rhs);
   virtual ~CrossProductOperator();

   virtual void dump(std::ostream& os, uint32_t lvl) const;

   virtual void open();
   virtual bool next();
   virtual void close();

private:
   std::unique_ptr<Operator> lhs;
   std::unique_ptr<Operator> rhs;
   bool lhsHasNext;
   OperatorState state;
};

}
