#pragma once

#include "HashMapSegment.hpp"
#include "common/Config.hpp"
#include "HashMapMetaPage.hpp"
#include "HashMapBucketPage.hpp"
#include "HashMapDirectoryPage.hpp"
#include "HashMapBucketPage.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "buffer_manager/BufferFrame.hpp"
#include <vector>
#include <cmath>
#include <utility>
#include <bitset>
#include <memory>

namespace dbi {

template<typename Key, typename Value>
class HashMap {
public:
   HashMap(HashMapSegment& segment);

   void insert(const Key& key, const Value& value);
   Value get(const Key& key);
   void remove(const Key& key);

   void dump(std::ostream& os);

private:
   HashMapSegment& segment;

   bool tryInsert(const HashMapMetaPage& meta, const Key& key, const Value& value);
   BufferFrame& getBucket(const HashMapMetaPage& meta, const Key& key);
   void doLinearGrow(const HashMapMetaPage& meta);
};

template<class Key, class Value>
HashMap<Key, Value>::HashMap(HashMapSegment& segment)
: segment(segment)
{
}

template<class Key, class Value>
void HashMap<Key, Value>::insert(const Key& key, const Value& value)
{
   // Fix meta page
   auto& metaBf = segment.getMetaBufferFrame();
   auto& metaPage = reinterpret_cast<HashMapMetaPage&>(*metaBf.data());

   // Insert value
   while(!tryInsert(metaPage, key, value))
      doLinearGrow(metaPage);

   // Update and unfix meta
   metaPage.entries++;
   segment.unfixPage(metaBf, kDirty);
}

template<class Key, class Value>
Value HashMap<Key, Value>::get(const Key& key) {
   // uint16_t dirIndex = computeDirectoryIndex(key);
   // return directory[dirIndex]->get(key);
   throw;
}

template<class Key, class Value>
void HashMap<Key, Value>::remove(const Key& key) {
   throw;
}

template<class Key, class Value>
void HashMap<Key, Value>::dump(std::ostream& os)
{
   // Get meta page
   auto& metaBf = segment.getMetaBufferFrame();
   auto& metaPage = reinterpret_cast<HashMapMetaPage&>(*metaBf.data());
   os << "Linear Hash Map" << std::endl;
   os << "==================================" << std::endl;
   os << "meta.nextFreePageInternalPageId: " << metaPage.nextFreePageInternalPageId << std::endl;
   os << "meta.numRelevantBits: " << (int)metaPage.numRelevantBits << std::endl;
   os << "meta.directoryPageCount: " << metaPage.directoryPageCount << std::endl;
   os << "meta.next: " << metaPage.next << std::endl;
   os << "meta.size: " << metaPage.size << std::endl;
   os << "meta.entries: " << metaPage.entries << std::endl;
   os << "==================================" << std::endl;

   // Loop over each directory entry
   for(uint32_t i=0; i<metaPage.directoryPageCount; i++) {
      // Get the directory page
      PageId directoryPageId = metaPage.getDirectoryPage(i);
      auto& directoryBf = segment.fixGlobalPage(directoryPageId, kShared);
      auto& directoryPage = reinterpret_cast<DirectoryPage&>(*directoryBf.data());
      uint32_t bucketsOnPage = metaPage.size - i*(kPageSize/sizeof(PageId));
      os << "Directory page " << directoryPageId << std::endl;
      os << "==================================" << std::endl;

      // Loop over all entries in this directory page
      for(uint32_t position=0; position<bucketsOnPage; position++) {
         // Get the bucket and dump it
         os << "Bucket " << position << std::endl;
         os << "-------------------" << std::endl;
         auto& bucketBf = segment.fixGlobalPage(directoryPage[position], kExclusive);
         auto& bucketPage = reinterpret_cast<HashMapBucketPage<Key,Value>&>(*bucketBf.data());
         bucketPage.dump(os);
         segment.unfixPage(bucketBf, kClean);
         os << "-------------------" << std::endl;
      }
      segment.unfixPage(directoryBf, kClean);
   }
   os << "==================================" << std::endl;
   segment.unfixPage(metaBf, kClean);
}

template<class Key, class Value>
bool HashMap<Key, Value>::tryInsert(const HashMapMetaPage& meta, const Key& key, const Value& value)
{
   // Get the bucket corresponding to the given key
   auto& bucketBf = getBucket(meta, key);
   auto& bucketPage = reinterpret_cast<HashMapBucketPage<Key,Value>&>(*bucketBf.data());
   bool success = bucketPage.insert(key, value);
   segment.unfixPage(bucketBf, kDirty);

   // Update meta data
   return success;
}

template<class Key, class Value>
BufferFrame& HashMap<Key, Value>::getBucket(const HashMapMetaPage& meta, const Key& key)
{
   // Obtain position
   uint32_t h = std::hash<Key>()(key);
   uint32_t smallMask = (1<<(meta.numRelevantBits-1))-1;
   uint32_t bigMask = (1<<(meta.numRelevantBits))-1;
   uint32_t pos = h & smallMask;
   if(pos > meta.next)
      pos = h & bigMask;

   // Map to a concrete page and an offset
   PageId directoryPageId = meta.getDirectoryPage(pos / (kPageSize/sizeof(PageId)));
   uint32_t offset = pos % (kPageSize/sizeof(PageId));

   // Get directory page
   auto& directoryBf = segment.fixGlobalPage(directoryPageId, kShared);
   auto& directoryPage = reinterpret_cast<DirectoryPage&>(*directoryBf.data());
   PageId bucketPageId = directoryPage[offset];
   segment.unfixPage(directoryBf, kClean);

   // Get bucket
   return segment.fixGlobalPage(bucketPageId, kExclusive);
}

template<class Key, class Value>
void HashMap<Key, Value>::doLinearGrow(const HashMapMetaPage&)
{
   std::cout << "doing grow.." << std::endl;
   throw;
}

}
