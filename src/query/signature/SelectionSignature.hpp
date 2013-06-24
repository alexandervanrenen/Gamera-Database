#pragma once

#include "query/parser/Common.hpp"
#include "query/util/Predicate.hpp"
#include "Signature.hpp"
#include <memory>
#include <vector>

namespace dbi {

class SelectionSignature : public Signature {
public:
   /// Create signature using the supplied signature of the underlying operator and the predicate
   SelectionSignature(const Signature& source, std::unique_ptr<qopt::Predicate> predicate, std::vector<harriet::Value>& golbalRegister);

   bool fullfillsPredicates(const std::vector<harriet::Value>& tuple) const;

   virtual void dump(std::ostream& os) const;

private:
   std::unique_ptr<qopt::Predicate> predicate;
   std::vector<harriet::Value>& golbalRegister;
};

}
