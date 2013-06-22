#pragma once

#include "harriet/ScriptLanguage.hpp"
#include <string>
#include <vector>

namespace dbi {

class ColumnSignature;

class Signature {
public:
   /// Constructor
   virtual ~Signature();

   /// Access the signatures attributes
   const std::vector<ColumnSignature>& getAttributes() const;

   /// Dump all attributes to os
   virtual void dump(std::ostream& os) const;

   /// Checks if there is a match (can be an ambiguous one)
   bool hasAttribute(const std::string& alias, const std::string& name) const;
   /// Finds the best matching attribute with given alias.name. throws when ambiguous or unknown
   const ColumnSignature& getAttribute(const std::string& alias, const std::string& columnName) const;
   const ColumnSignature& getAttribute(uint32_t tableIndex, const std::string& columnName) const;

protected:
   ///
   static const ColumnSignature& getAttribute(const std::vector<ColumnSignature>& attributes, const std::string& alias, const std::string& columnName);
   static const ColumnSignature& getAttribute(const std::vector<ColumnSignature>& attributes, uint32_t tableIndex, const std::string& columnName);

   /// A signature consists of attributes
   std::vector<ColumnSignature> attributes;
};

}
