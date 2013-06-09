#include "Signature.hpp"
#include "harriet/Expression.hpp"

using namespace std;

namespace dbi {

Signature::Signature()
{
}

Signature::Signature(const RelationSchema& relationSchema, const string& alias)
{
   attributes.reserve(relationSchema.getAttributes().size());
   for(auto& attribute : relationSchema.getAttributes())
      attributes.push_back(AttributeSignature{attribute.name, alias, attribute.notNull, attribute.primaryKey, attribute.type});
}

Signature::Signature(const vector<unique_ptr<harriet::Value>>& values)
{
   for(auto& iter : values)
      attributes.push_back(AttributeSignature{"", "", true, true, iter->getResultType()});
}

Signature Signature::createProjectionSignature(const vector<ColumnIdentifier>& target) const
{
   vector<uint32_t> projection = createProjection(target);
   Signature result;
   for(uint32_t i=0; i<projection.size(); i++) {
      result.attributes.push_back(attributes[projection[i]]);
      result.attributes.back().name = attributes[projection[i]].name;
      result.attributes.back().alias = attributes[projection[i]].alias;
   }
   return result;
}

vector<uint32_t> Signature::createProjection(const vector<ColumnIdentifier>& target) const
{
   vector<uint32_t> result;
   for(auto& iter : target)
      result.push_back(getAttribute(iter.tableIdentifier, iter.columnIdentifier));
   return result;
}

const vector<AttributeSignature>& Signature::getAttributes() const
{
   return attributes;
}

void Signature::dump(std::ostream& os) const
{
   for(uint32_t i=0; i<attributes.size(); i++)
      os << attributes[i].alias << "." << attributes[i].name << (i+1!=attributes.size()?" | ":"");
}

uint32_t Signature::getAttribute(const string& alias, const string& name) const
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

   throw harriet::Exception{"unknown identifier: '" + alias + "." + name + "'"};
}

}
