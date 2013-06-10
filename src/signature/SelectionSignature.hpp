#pragma once

#include "Signature.hpp"
#include "schema/Common.hpp"

namespace dbi {

class SelectionSignature : public Signature {
public:
   /// Create signature using the supplied signature of the underlying operator and the predicate
   SelectionSignature(const Signature& source, std::unique_ptr<harriet::Expression> expression);

   bool fullfillsPredicates(const std::vector<std::unique_ptr<harriet::Value>>& tuple);

   virtual void dump(std::ostream& os) const;

private:
   std::unique_ptr<harriet::Expression> selectionCondition;

   struct VariableMapping {
      std::string name; // Name of the variable
      uint32_t position; // Position in the tuple
   };
   std::vector<VariableMapping> variableMapping;

   enum struct Type : uint8_t {kUnknown, kConstant, kOneColumn, kTwoColumn, kComplex};
   Type type;

   std::vector<VariableMapping> getFreeVariables(const harriet::Expression& expression) const;
};

}
