#include "IndexKey.hpp"
#include "harriet/VariableType.hpp"
#include "harriet/Value.hpp"
#include "IndexKeySchema.hpp"
#include <iostream>

using namespace std;

namespace dbi {

IndexKey::IndexKey(vector<harriet::Value>&& valuesIn)
: values(move(valuesIn))
{
   valueReferences.resize(values.size());
   for(uint32_t i=0; i<values.size(); i++)
      valueReferences[i] = &values[i];
}

IndexKey::IndexKey(vector<harriet::Value*> valueReferences)
: valueReferences(valueReferences)
{
}

IndexKey::IndexKey(IndexKey&& other)
: values(move(other.values))
, valueReferences(move(valueReferences))
{
}

IndexKey& IndexKey::operator =(IndexKey&& other)
{
   values = move(other.values);
   valueReferences = move(valueReferences);
   return *this;
}

IndexKey::~IndexKey()
{
}

uint64_t IndexKey::bytes() const
{
   uint64_t result = 0;
   for(auto iter : valueReferences)
      result += iter->type.length;
   return result;
}

void IndexKey::writeToMem(char* mem) const
{
   for(auto iter : valueReferences) {
      iter->marschall(mem);
      mem += iter->type.length;
   }
}

void IndexKey::readFromMemory(const char* ptr)
{
   for(auto iter : valueReferences) {
      *iter = harriet::Value::createFromRecord(iter->type, ptr);
      ptr += iter->type.length;
   }
}

IndexKey IndexKey::readFromMemory(const char* ptr, const IndexKeySchema& schema)
{
   vector<harriet::Value> values;
   for(auto iter : schema.getSchema()) {
      values.emplace_back(harriet::Value::createFromRecord(iter, ptr));
      ptr += iter.length;
   }
   return IndexKey(move(values));
}

void IndexKey::dump(ostream& os, harriet::Value* offset) const
{
   for(auto iter : valueReferences)
      os << " " << iter-offset;
}

}
