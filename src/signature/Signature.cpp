#include "Signature.hpp"
#include "harriet/Expression.hpp"
#include <sstream>

using namespace std;

namespace dbi {

const vector<AttributeSignature>& Signature::getAttributes() const
{
   return attributes;
}

void Signature::dump(std::ostream& os) const
{
   os << "[ ";
   for(uint32_t i=0; i<attributes.size(); i++)
      os << attributes[i].alias << "." << attributes[i].name << (i+1!=attributes.size()?" | ":"");
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

uint32_t Signature::getAttributeIndex(const string& alias, const string& name) const
{
   uint32_t resultIndex = attributes.size(); // invalid index

   // Try to find a matching identifier
   if(alias != "") {
      for(uint32_t i=0; i<attributes.size(); i++)
         if(attributes[i].name==name && alias==attributes[i].alias) {
            if(resultIndex==attributes.size())
               resultIndex = i; else
               throw harriet::Exception{"ambiguous identifier '" + alias + "." + name + "', candidates: '" + attributes[i].alias + "." + attributes[i].name + "' or '" + attributes[resultIndex].alias + "." + attributes[resultIndex].name + "'"};
         }
   } else {
      for(uint32_t i=0; i<attributes.size(); i++)
         if(attributes[i].name==name) {
            if(resultIndex==attributes.size())
               resultIndex = i; else
               throw harriet::Exception{"ambiguous identifier '" + alias + "." + name + "', candidates: '" + attributes[i].alias + "." + attributes[i].name + "' or '" + attributes[resultIndex].alias + "." + attributes[resultIndex].name + "'"};
         }
   }

   if(resultIndex != attributes.size())
      return resultIndex;

   ostringstream os;
   dump(os);
   throw harriet::Exception{"unknown identifier: '" + alias + "." + name + "' \ncandidates are: " + (os.str().size()==0?"<none>":os.str())};
}

}
