#include "ExtentStore.hpp"
#include "util/StupidUserException.hpp"
#include <algorithm>
#include <exception>
#include <iostream>
#include <vector>

using namespace std;

namespace dbi {

ExtentStore::ExtentStore()
{
}

ExtentStore::ExtentStore(ExtentStore&& other)
: pageCount(other.pageCount)
, extents(move(other.extents))
{
}

const ExtentStore& ExtentStore::operator=(ExtentStore&& other)
{
   extents = move(other.extents);
   pageCount = other.pageCount;
   return *this;
}

void ExtentStore::add(const Extent& extent)
{
   assert(extent.numPages() > 0);

   // The new extent is not allowed to be inside an already existing extent
   for(auto iter : extents)
      if( (iter.begin().toInteger() <= extent.begin().toInteger() && extent.begin().toInteger() < iter.end().toInteger())
       || (iter.begin().toInteger() < extent.end().toInteger() && extent.end().toInteger() <= iter.end().toInteger()))
         throw util::StupidUserException("extent store: overlapping extents");

   // Add extent
   pageCount += extent.numPages();
   if(!extents.empty() && extents.back().end() == extent.begin())
      extents.back() = Extent(extents.back().begin(), extent.end()); else // Merge to last extent
      extents.emplace_back(extent); // Add a new extent
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
