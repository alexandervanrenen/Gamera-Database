#pragma once

#include "common/Config.hpp"
#include <cassert>
#include <ios>

namespace dbi {

/// A page range. Stores at most 2^32 pages
/// Assuming 16 KB pages one extent can hold about 64 TB
struct Extent {
   Extent(PageId begin, PageId end) : beginPage(begin), pageCount(end.toInteger()-begin.toInteger()) {assert(static_cast<uint32_t>(end.toInteger()-begin.toInteger()) == end.toInteger()-begin.toInteger());}

   uint32_t numPages() const {return pageCount;}

   PageId begin() const {return beginPage;}
   PageId end() const {return PageId(beginPage.toInteger()+pageCount);}

   bool operator==(const Extent& other) const {return beginPage==other.beginPage && pageCount==other.pageCount;}
   friend std::ostream& operator<<(std::ostream& out, const Extent& e);

private:
   // Pages belonging to this extent: [begin, end)
   PageId beginPage;
   uint64_t pageCount;
};

}
