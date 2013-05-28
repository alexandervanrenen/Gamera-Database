#pragma once

#include "common/Config.hpp"
#include "Extent.hpp"
#include <cassert>
#include <vector>

namespace dbi {

/// Define PageID iterator -- use to iterate over all page ids of this segment
/// Changing the extents of the segment potentially breaks the iterator
class PageIDIterator : public std::iterator<std::forward_iterator_tag, PageIDIterator> {
public:
   PageIDIterator(const PageIDIterator& other)
   : extent(other.extent)
   , pageID(other.pageID)
   , extents(other.extents)
   {
   }

   PageIDIterator& operator=(const PageIDIterator& other)
   {
      extent = other.extent;
      pageID = other.pageID;
      extents = other.extents;
      return *this;
   }

   const PageIDIterator operator++()
   {
      PageIDIterator result = *this;
      inc();
      return result;
   }

   const PageIDIterator& operator++(int)
   {
      inc();
      return *this;
   }

   bool operator==(const PageIDIterator& other) const
   {
      return pageID == other.pageID;
   }

   bool operator!=(const PageIDIterator& other) const
   {
      return pageID != other.pageID;
   }

   PageId operator*() const
   {
      return pageID;
   }

private:
   uint32_t extent;
   PageId pageID;
   const std::vector<Extent>* extents;

   PageIDIterator(const std::vector<Extent>& extents, PageId pageID)
   : extent(0), pageID(pageID), extents(&extents)
   {
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

   friend class Segment;
};

}
