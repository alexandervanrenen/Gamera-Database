#pragma once

#include "common/Config.hpp"
#include <cassert>

namespace dbi {

/// A page range. Stores at most 2^32 pages
/// Assuming 16 KB pages one extent can hold about 64 TB
struct Extent {
   Extent(PageId begin, PageId end) : beginPage(begin), pageCount(end-begin) {assert(static_cast<uint32_t>(end-begin) == end-begin);}

   uint32_t numPages() const {return pageCount;}

   PageId begin() const {return beginPage;}
   PageId end() const {return beginPage+pageCount;}

private:
   // Pages belonging to this extent: [begin, end)
   PageId beginPage;
   uint32_t pageCount;
};

}
