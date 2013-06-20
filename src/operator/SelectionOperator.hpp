#pragma once

#include "Operator.hpp"
#include "OperatorState.hpp"
#include "signature/SelectionSignature.hpp"
#include "harriet/Value.hpp"
#include <cstdint>
#include <memory>

namespace dbi {

namespace qopt { class Predicate; }

/// 
class SelectionOperator : public Operator {
public:
   SelectionOperator(std::unique_ptr<Operator> source, std::unique_ptr<qopt::Predicate> predicate, std::vector<harriet::Value>& globalRegister);
   virtual ~SelectionOperator();

   virtual const Signature& getSignature() const;
   virtual void prepare(std::vector<harriet::Value>& globalRegister, const std::set<qopt::ColumnAccessInfo>& requiredColumns);
   virtual void dump(std::ostream& os, uint32_t lvl) const;

   virtual void open();
   virtual bool next();
   virtual void close();

private:
   std::unique_ptr<Operator> source;
   OperatorState state;
   SelectionSignature signature;
   std::vector<harriet::Value>& globalRegister;
};

}
