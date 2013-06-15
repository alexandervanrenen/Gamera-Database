#pragma once

#include "Signature.hpp"

namespace dbi {

class SelectionSignature : public Signature {
public:
   /// Create signature using the supplied signature of the underlying operator and the predicate
   SelectionSignature(const Signature& source, std::unique_ptr<harriet::Expression> expression);

   bool fullfillsPredicates(const std::vector<harriet::Value>& tuple);

   virtual void dump(std::ostream& os) const;

private:
   std::unique_ptr<harriet::Expression> selectionCondition;

   struct VariableMapping {
      std::string name; // Name of the variable
      uint32_t position; // Position in the tuple
   };
   std::vector<VariableMapping> variableMapping;

   enum struct Type : uint8_t {kConstant, kOneColumn, kTwoColumn, kComplex, kUnknown};
   Type type;

   std::vector<VariableMapping> getFreeVariables(const harriet::Expression& expression) const;
};

}
