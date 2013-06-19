#include "TableScanSignature.hpp"
#include "schema/RelationSchema.hpp"

using namespace std;

namespace dbi {

TableScanSignature::TableScanSignature(const RelationSchema& relationSchema, const string& alias)
{
   attributes.reserve(relationSchema.getAttributes().size());
   for(auto& attribute : relationSchema.getAttributes())
      attributes.push_back(AttributeSignature{attribute.name, alias, attribute.notNull, attribute.type});
}

}
