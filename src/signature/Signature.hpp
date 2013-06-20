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
   uint32_t index; // In global register
   uint32_t tableIndex; // Table id
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
   const AttributeSignature& getAttribute(const std::string& alias, const std::string& columnName) const;
   const AttributeSignature& getAttribute(uint32_t tableIndex, const std::string& columnName) const;

protected:
   ///
   static const AttributeSignature& getAttribute(const std::vector<AttributeSignature>& attributes, const std::string& alias, const std::string& columnName);
   static const AttributeSignature& getAttribute(const std::vector<AttributeSignature>& attributes, uint32_t tableIndex, const std::string& columnName);

   /// A signature consists of attributes
   std::vector<AttributeSignature> attributes;
};

}
