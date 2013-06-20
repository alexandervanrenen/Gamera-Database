#pragma once

#include "Signature.hpp"
#include "query_parser/Common.hpp"
#include "query_util/Predicate.hpp"
#include <vector>
#include <memory>

namespace dbi {

class SelectionSignature : public Signature {
public:
   /// Create signature using the supplied signature of the underlying operator and the predicate
   SelectionSignature(const Signature& source, std::unique_ptr<qopt::Predicate> predicate);

   bool fullfillsPredicates(const std::vector<harriet::Value>& tuple) const;

   virtual void dump(std::ostream& os) const;

private:
   std::unique_ptr<qopt::Predicate> predicate;

   struct VariableMapping {
      std::string name; // Name of the variable
      uint32_t position; // Position in the tuple
   };
   std::vector<VariableMapping> variableMapping;

   enum struct Type : uint8_t {kConstant, kOneColumn, kTwoColumn, kComplex, kUnknown};
   Type type;

   std::vector<VariableMapping> getFreeVariables(std::unique_ptr<harriet::Expression>& expression) const;
};

}
