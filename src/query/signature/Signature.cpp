#include "Signature.hpp"
#include "harriet/Expression.hpp"
#include "query/signature/ColumnSignature.hpp"
#include <sstream>

using namespace std;

namespace dbi {

Signature::~Signature()
{

}

const vector<ColumnSignature>& Signature::getAttributes() const
{
   return attributes;
}

void Signature::dump(ostream& os) const
{
   os << "[ ";
   for(uint32_t i=0; i<attributes.size(); i++)
      os << attributes[i].alias << "." << attributes[i].name << "(" << attributes[i].index << ")" << (i+1!=attributes.size()?" | ":"");
   os << " ]";
}

bool Signature::hasAttribute(const string& alias, const string& name) const
{
   // Try to find any matching identifier
   for(uint32_t i=0; i<attributes.size(); i++)
      if(attributes[i].name==name && (alias.size()==0 || alias==attributes[i].alias))
         return true;
   return false;
}

const ColumnSignature& Signature::getAttribute(const string& alias, const string& columnName) const
{
   return getAttribute(attributes, alias, columnName);
}

const ColumnSignature& Signature::getAttribute(uint32_t tableIndex, const string& columnName) const
{
   return getAttribute(attributes, tableIndex, columnName);
}

const ColumnSignature& Signature::getAttribute(const vector<ColumnSignature>& attributes, const string& alias, const string& columnName)
{
   uint32_t resultIndex = attributes.size(); // invalid index

   // Try to find a matching identifier
   if(alias != "") {
      for(uint32_t i=0; i<attributes.size(); i++)
         if(attributes[i].name==columnName && alias==attributes[i].alias) {
            if(resultIndex==attributes.size())
               resultIndex = i; else
               throw harriet::Exception{"ambiguous identifier '" + alias + "." + columnName + "', candidates: '" + attributes[i].alias + "." + attributes[i].name + "' or '" + attributes[resultIndex].alias + "." + attributes[resultIndex].name + "'"};
         }
   } else {
      for(uint32_t i=0; i<attributes.size(); i++)
         if(attributes[i].name==columnName) {
            if(resultIndex==attributes.size())
               resultIndex = i; else
               throw harriet::Exception{"ambiguous identifier '" + alias + "." + columnName + "', candidates: '" + attributes[i].alias + "." + attributes[i].name + "' or '" + attributes[resultIndex].alias + "." + attributes[resultIndex].name + "'"};
         }
   }

   if(resultIndex != attributes.size())
      return attributes[resultIndex];

   throw harriet::Exception{"unknown identifier: '" + alias + "." + columnName + "'"};
}

const ColumnSignature& Signature::getAttribute(const vector<ColumnSignature>& attributes, uint32_t tableIndex, const string& columnName)
{
   uint32_t resultIndex = attributes.size(); // invalid index
   for(uint32_t i=0; i<attributes.size(); i++)
      if(attributes[i].name==columnName && attributes[i].tableIndex==tableIndex) {
         if(resultIndex==attributes.size())
            resultIndex = i; else
            throw harriet::Exception{"ambiguous identifier '" + to_string(tableIndex) + "." + columnName + "', candidates: '" + attributes[i].alias + "." + attributes[i].name + "' or '" + attributes[resultIndex].alias + "." + attributes[resultIndex].name + "'"};
      }

   if(resultIndex != attributes.size())
      return attributes[resultIndex];

   throw harriet::Exception{"unknown identifier: '" + to_string(tableIndex) + "." + columnName + "'"};  
}

}
