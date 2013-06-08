#include "Signature.hpp"
#include "harriet/Expression.hpp"

using namespace std;

namespace dbi {

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

const vector<AttributeSignature>& Signature::getAttributes() const
{
   return attributes;
}

}
