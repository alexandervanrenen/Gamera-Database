#pragma once

#include "Extent.hpp"
#include <vector>

namespace dbi {

namespace util {

/// Takes care of storing extents in order and merged: [3,4[ and [4,5[ => [3,5[
/// Think of extents as ranges. Given the extent [1,100[ removing [2,5[ results in [1,1[ and [5,100[
/// Illegal instructs will be prevented:
/// -> Its not OK to remove [200,300[ from [1,100[
/// -> Its not OK to add [50,100[ to [1,100[
class ExtentStore {
public:
   /// Constructor and move operations -- the rest is default
   ExtentStore();
   ExtentStore(ExtentStore&& other);
   const ExtentStore& operator=(ExtentStore&& other);

   /// Merge given extent into existing ones
   void add(const Extent& extent);
   /// Remove the given extent
   void remove(const Extent& extent);
   /// Access the ordered extents
   const std::vector<Extent>& get() const;
   /// Sum of all extents
   uint64_t numPages() const;

private:
   uint64_t pageCount = 0;
   std::vector<Extent> extents;
};

}

}
