#pragma once

#include "Signature.hpp"
#include "schema/Common.hpp"

namespace dbi {

class SelectionSignature : public Signature {
public:
   /// Create signature using the supplied signature of the underlying operator and the predicate
   SelectionSignature(const Signature& source, const Predicate& predicate);

   bool fullfillsPredicates(const std::vector<std::unique_ptr<harriet::Value>>& tuple);

private:
   std::vector<Predicate> predicates;
   uint32_t lhs;
   uint32_t rhs;
};

}
