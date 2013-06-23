#include "IndexKeySchema.hpp"
#include "harriet/VariableType.hpp"

using namespace std;

namespace dbi {

IndexKeySchema::IndexKeySchema(const vector<harriet::VariableType>& schema)
: schema(schema)
{
}

const vector<harriet::VariableType> IndexKeySchema::getSchema() const
{
   return schema;
}

}
