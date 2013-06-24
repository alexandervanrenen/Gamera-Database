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

uint64_t IndexKeySchema::bytes() const
{
   uint64_t result = 0;
   for(auto iter : schema)
      result += iter.length;
   return result;
}

}
