#include "ExtentStore.hpp"
#include "util/StupidUserException.hpp"
#include <vector>
#include <iostream>
#include <exception>
#include <algorithm>

using namespace std;

namespace dbi {

ExtentStore::ExtentStore()
{
}

ExtentStore::ExtentStore(ExtentStore&& other)
: extents(move(other.extents))
{
}

const ExtentStore& ExtentStore::operator=(ExtentStore&& other)
{
   extents = move(other.extents);
   return *this;
}

void ExtentStore::add(const Extent& extent)
{
   assert(extent.numPages() > 0);

   // The new extent is not allowed to be inside an already existing extent
   for(auto iter : extents)
      if( (iter.begin() <= extent.begin() && extent.begin() < iter.end())
       || (iter.begin() < extent.end() && extent.end() <= iter.end()))
         throw util::StupidUserException("extent store: overlapping extents");

   // Add extent
   pageCount += extent.numPages();
   extents.emplace_back(extent);
   sort(extents.begin(), extents.end(), [](const Extent& lhs, const Extent& rhs){return lhs.begin() < rhs.begin();});

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

void ExtentStore::remove(const Extent& extent)
{
   assert(extent.numPages() > 0);

   // The has to be completely inside an already existing one (iterators are not stable)
   for(uint32_t i=0; i<extents.size(); i++)
      if(extents[i].begin() <= extent.begin() && extent.end() <= extents[i].end()) {
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

const vector<Extent>& ExtentStore::get() const
{
   return extents;
}

uint64_t ExtentStore::numPages() const
{
   return pageCount;
}

}
