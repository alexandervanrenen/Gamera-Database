#pragma once

#include "harriet/ScriptLanguage.hpp"
#include <vector>
#include <string>

namespace dbi {

struct AttributeSignature {
   std::string name;
   std::string alias;
   bool notNull;
   harriet::VariableType type;
};

class Signature {
public:
   /// Access the signatures attributes
   const std::vector<AttributeSignature>& getAttributes() const;

   /// Dump all attributes to os
   virtual void dump(std::ostream& os) const;

   /// Checks if there is a match (can be an ambiguous one)
   bool hasAttribute(const std::string& alias, const std::string& name) const;
   /// Finds the best matching attribute with given alias.name. throws when ambiguous or unknown
   uint32_t getAttributeIndex(const std::string& alias, const std::string& name) const;

protected:
   /// A signature consists of attributes
   std::vector<AttributeSignature> attributes;
};

}
