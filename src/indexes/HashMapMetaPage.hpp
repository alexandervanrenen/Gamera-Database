#pragma once

#include "segment_manager/Extent.hpp"
#include "common/Config.hpp"
#include <cstdint>

namespace dbi {

/// Keeps track of meta data and stores and controls all directory pages in extent form.
class HashMapMetaPage {
public:
   void initialize()
   {
      assert(directoryExtent.numPages() == 1);
      header.nextFreeInternalPageId = 1; // I am zero
      header.directoryPageCount = 0;
      header.next = 0;
      header.size = 0;
      header.entryCount = 0;
      directoryExtents[0] = directoryExtent;
   }

   void addDirectoryExtent(const Extent& directoryExtent)
   {
      
   }

   void addBucketExtent(const Extent& bucketExtent)
   {
      
   }

   PageId createDirectoryPage()
   {

   }

   PageId createBucketPage()
   {

   }

   bool growDirectory()
   {
      // In which page is the 
      uint64_t directoryEntries = header.size + header.next;
      uint64_t directoryPageIndex = directoryEntries / kEntriesPerDirectoryPage;

      // Transform to index inside the current extent
      for(uint32_t i=0; i<header.directoryExtentCount; i++) {
         assert(directoryPageIndex > header.directoryExtents[i].numPages());
         directoryPageIndex -= header.directoryExtents[i].numPages();
      }

      // Need more pages for the directories
      if(directoryPageIndex == header.nextFreeDirectoryPage) {


         Extent currentExtent = header.directoryExtents[header.directoryExtentCount-1];
         if(currentExtent.end() < )
         
      }

      if(header.next == header.size) {
         next = 0;
         size = std::max(1, 2*header.size);
      }
   }

   uint64_t getDirectoryPage(uint64_t pos)
   {
      // Find extent for the requested offset
      for(auto& iter : directoryExtents)
         if(iter.numPages() > pos)
            return PageId(iter.begin().toInteger() + pos);
         else
            pos -= iter.numPages();
      throw;
   }

   struct {
      uint64_t next;
      uint64_t size;
      uint64_t entryCount;

      uint16_t directoryExtentCount; // Marks current extent
      uint16_t bucketExtentCount; // Marks current extent

      uint64_t nextFreeDirectoryPage; // Relative to current extent
      uint64_t nextFreeBucketPage; // Relative to current extent
   } header;

private:
   std::array<Extent, (kPageSize-sizeof(header)) / sizeof(Extent) / 2> directoryExtents;
   std::array<Extent, (kPageSize-sizeof(header)) / sizeof(Extent) / 2> bucketExtents;

   static const uint64_t kEntriesPerDirectoryPage = kPageSize/sizeof(PageId);
};

}
