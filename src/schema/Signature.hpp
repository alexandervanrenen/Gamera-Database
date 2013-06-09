#pragma once

#include "RelationSchema.hpp"
#include "harriet/ScriptLanguage.hpp"
#include "schema/Common.hpp"
#include <vector>
#include <string>

namespace dbi {

struct AttributeSignature {
   std::string name;
   std::string alias;
   bool notNull;
   bool primaryKey;
   harriet::VariableType type;
};

class Signature {
public:
   // Empty signature
   Signature();
   // Create named variables from a TableScanOperator
   Signature(const RelationSchema& relationSchema, const std::string& alias);
   // Deduce schema from an expression in a script. For the SingleRecordOperator
   Signature(const std::vector<std::unique_ptr<harriet::Value>>& values);
   /// For ProjectionOperator
   Signature createProjectionSignature(const std::vector<ColumnIdentifier>& target) const;
   std::vector<uint32_t> createProjection(const std::vector<ColumnIdentifier>& target) const;

   /// Access the signatures attributes
   const std::vector<AttributeSignature>& getAttributes() const;

   /// Dump all attributes to os
   void dump(std::ostream& os) const;

private:
   /// Finds the best matching attribute with given alias.name. throws when ambiguous or unknown
   uint32_t getAttribute(const std::string& alias, const std::string& name) const;

   /// A signature consists of attributes
   std::vector<AttributeSignature> attributes;
};

}
