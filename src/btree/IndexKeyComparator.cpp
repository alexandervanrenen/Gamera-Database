#include "IndexKeyComparator.hpp"
#include "IndexKeySchema.hpp"
#include "IndexKey.hpp"
#include "harriet/Value.hpp"
#include <cassert>

namespace dbi {

IndexKeyComparator::IndexKeyComparator(const IndexKeySchema& schema)
: schema(schema)
{
}

bool IndexKeyComparator::less(const IndexKey& lhs, const IndexKey& rhs) const
{
   assert(lhs.valueReferences.size()==rhs.valueReferences.size() && lhs.valueReferences.size()==schema.getSchema().size());

   for(uint32_t i=0; i<schema.getSchema().size(); i++)
      if(lhs.valueReferences[i]->computeLt(*rhs.valueReferences[i]).data.vbool)
         return true;
      else if(lhs.valueReferences[i]->computeEq(*rhs.valueReferences[i]).data.vbool)
         continue;
      else
         return false;
   return false;
}

bool IndexKeyComparator::less(const IndexKey& lhs, const char* rhs) const
{
   return less(lhs, IndexKey::readFromMemory(rhs, schema.getSchema()));
}

bool IndexKeyComparator::less(const char* lhs, const IndexKey& rhs) const
{
   return less(IndexKey::readFromMemory(lhs, schema.getSchema()), rhs);
}

bool IndexKeyComparator::less(const char* lhs, const char* rhs) const
{
   return less(IndexKey::readFromMemory(lhs, schema.getSchema()), IndexKey::readFromMemory(rhs, schema.getSchema()));
}

bool IndexKeyComparator::equal(const IndexKey& lhs, const IndexKey& rhs) const
{
   assert(lhs.valueReferences.size()==rhs.valueReferences.size() && lhs.valueReferences.size()==schema.getSchema().size());

   for(uint32_t i=0; i<schema.getSchema().size(); i++)
      if(lhs.valueReferences[i]->computeNeq(*rhs.valueReferences[i]).data.vbool)
         return false;
   return true;
}

bool IndexKeyComparator::equal(const IndexKey& lhs, const char* rhs) const
{
   return equal(lhs, IndexKey::readFromMemory(rhs, schema.getSchema()));
}

bool IndexKeyComparator::equal(const char* lhs, const IndexKey& rhs) const
{
   return equal(IndexKey::readFromMemory(lhs, schema.getSchema()), rhs);
}

bool IndexKeyComparator::equal(const char* lhs, const char* rhs) const
{
   return equal(IndexKey::readFromMemory(lhs, schema.getSchema()), IndexKey::readFromMemory(rhs, schema.getSchema()));
}

}
