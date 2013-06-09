#pragma once

#include "Operator.hpp"
#include "OperatorState.hpp"
#include "signature/SelectionSignature.hpp"
#include "schema/Common.hpp"
#include <cstdint>
#include <memory>

namespace dbi {

/// 
class SelectionOperator : public Operator {
public:
   SelectionOperator(std::unique_ptr<Operator> source, const Predicate& predicate);
   virtual ~SelectionOperator();

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
   SelectionSignature signature;
   std::vector<std::unique_ptr<harriet::Value>> tuple;
};

}
