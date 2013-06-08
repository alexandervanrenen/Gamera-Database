#pragma once

#include "RelationSchema.hpp"
#include "harriet/ScriptLanguage.hpp"
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
   Signature(const RelationSchema& relationSchema, const std::string& alias); // Create named variables from a table scan
   Signature(const std::vector<std::unique_ptr<harriet::Value>>& values); // Deduce schema from an expression in a script

   const std::vector<AttributeSignature>& getAttributes() const;
private:
   std::vector<AttributeSignature> attributes;
};

}
