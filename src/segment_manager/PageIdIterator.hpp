#pragma once

#include "common/Config.hpp"
#include "Extent.hpp"
#include <cassert>
#include <vector>

namespace dbi {

/// Define PageID iterator -- use to iterate over all page ids of this segment
/// Changing the extents of the segment potentially breaks the iterator
class PageIdIterator : public std::iterator<std::forward_iterator_tag, PageIdIterator> {
public:
   PageIdIterator(const PageIdIterator& other)
   : extent(other.extent)
   , pageID(other.pageID)
   , extents(other.extents)
   {
   }

   PageIdIterator& operator=(const PageIdIterator& other)
   {
      extent = other.extent;
      pageID = other.pageID;
      extents = other.extents;
      return *this;
   }

   const PageIdIterator operator++()
   {
      PageIdIterator result = *this;
      inc();
      return result;
   }

   const PageIdIterator& operator++(int)
   {
      inc();
      return *this;
   }

   const PageIdIterator& operator+=(uint64_t count)
   {
      inc(count);
      return *this;
   }

   bool operator==(const PageIdIterator& other) const
   {
      return pageID == other.pageID;
   }

   bool operator!=(const PageIdIterator& other) const
   {
      return pageID != other.pageID;
   }

   PageId operator*() const
   {
      return pageID;
   }

   friend const PageIdIterator& min(const PageIdIterator& lhs, const PageIdIterator& rhs)
   {
      if(lhs.extent < rhs.extent)
         return lhs;
      if(lhs.extent > rhs.extent)
         return rhs;
      if(lhs.pageID.toInteger() < rhs.pageID.toInteger())
         return lhs;
      return rhs;
   }

private:
   uint32_t extent;
   PageId pageID;
   const std::vector<Extent>* extents;

   PageIdIterator(const std::vector<Extent>& extents, PageId target)
   : extent(0), pageID(kInvalidPageID), extents(&extents)
   {
      for(;extent<extents.size(); extent++)
         if(extents[extent].begin().toInteger() <= target.toInteger() && target.toInteger() < extents[extent].end().toInteger()) {
            pageID = target;
            return;
         }
   }

   void inc()
   {
      assert(pageID != kInvalidPageID);
      pageID++;
      if(pageID == (*extents)[extent].end()) {
         extent++;
         if(extent >= extents->size())
            pageID = kInvalidPageID;
         else
            pageID = (*extents)[extent].begin();
      }
   }

   void inc(uint64_t count)
   {
      assert(pageID != kInvalidPageID);
      while(count!=0 && pageID!=kInvalidPageID) {
         // Next extent
         if(pageID == (*extents)[extent].end()) {
            extent++;
            if(extent >= extents->size())
               pageID = kInvalidPageID;
            else
               pageID = (*extents)[extent].begin();
         }
         // 
         uint64_t leftInThisExtent = (*extents)[extent].end().toInteger() - pageID.toInteger();
         if(leftInThisExtent > count) {
            pageID = PageId(pageID.toInteger() + count);
            count = 0;
         } else {
            count -= leftInThisExtent;
            pageID = (*extents)[extent].end();
         }
      }
   }

   friend class Segment;
};


}
