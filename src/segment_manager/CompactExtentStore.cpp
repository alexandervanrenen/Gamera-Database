#include "CompactExtentStore.hpp"
#include "util/StupidUserException.hpp"
#include <vector>
#include <iostream>
#include <exception>
#include <algorithm>

using namespace std;

namespace dbi {

CompactExtentStore::CompactExtentStore()
{
}

CompactExtentStore::CompactExtentStore(CompactExtentStore&& other)
: pageCount(other.pageCount)
, extents(move(other.extents))
{
}

const CompactExtentStore& CompactExtentStore::operator=(CompactExtentStore&& other)
{
   extents = move(other.extents);
   pageCount = other.pageCount;
   return *this;
}

void CompactExtentStore::add(const Extent& extent)
{
   assert(extent.numPages() > 0);

   // The new extent is not allowed to be inside an already existing extent
   for(auto iter : extents)
      if( (iter.begin().toInteger() <= extent.begin().toInteger() && extent.begin().toInteger() < iter.end().toInteger())
       || (iter.begin().toInteger() < extent.end().toInteger() && extent.end().toInteger() <= iter.end().toInteger()))
         throw util::StupidUserException("extent store: overlapping extents");

   // Add extent
   pageCount += extent.numPages();
   extents.emplace_back(extent);
   sort(extents.begin(), extents.end(), [](const Extent& lhs, const Extent& rhs){return lhs.begin().toInteger() < rhs.begin().toInteger();});

   // Merge
   for(uint32_t i=0; i<extents.size()-1; i++) {
      if(extents[i].end() == extents[i+1].begin()) {
         extents[i] = Extent(extents[i].begin(), extents[i+1].end());
         extents.erase(extents.begin()+i+1);
         i--;
         continue;
      }
   }
}

void CompactExtentStore::remove(const Extent& extent)
{
   assert(extent.numPages() > 0);

   // The has to be completely inside an already existing one (iterators are not stable)
   for(uint32_t i=0; i<extents.size(); i++)
      if(extents[i].begin().toInteger() <= extent.begin().toInteger() && extent.end().toInteger() <= extents[i].end().toInteger()) {
         pageCount -= extent.numPages();
         // Is there a range before the removed extent ?
         if(extents[i].end() != extent.end())
            extents.insert(extents.begin()+i+1, Extent(extent.end(), extents[i].end()));
         if(extents[i].begin() != extent.begin())
            extents.insert(extents.begin()+i+1, Extent(extents[i].begin(), extent.begin()));
         extents.erase(extents.begin()+i);
         return;
      }

   throw util::StupidUserException("extent store: removing extent which does not belong to this store");
}

const vector<Extent>& CompactExtentStore::get() const
{
   return extents;
}

uint64_t CompactExtentStore::numPages() const
{
   return pageCount;
}

}
