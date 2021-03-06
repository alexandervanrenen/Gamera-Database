#pragma once

#include "common/Config.hpp"
#include "Extent.hpp"
#include "Record.hpp"
#include <unordered_map>
#include <vector>

namespace dbi {

class BufferManager;
class ExtentStore;
class CompactExtentStore;

/// Persist all SegmentId -> vector<Extent> mappings
/// Uses linked list like structure beginning at kMetaPageId
class Persister {
public:
   Persister(BufferManager& bufferManager, CompactExtentStore& freePages); // Free pages are loaded and used when the persister needs a new page itself
   void create();
   void load(std::unordered_map<SegmentId, std::pair<TupleId, ExtentStore>>& segmentMap, SegmentId& nextFreeId);

   TupleId insert(SegmentId sid, const ExtentStore& extents);
   TupleId update(TupleId tid, SegmentId sid, const ExtentStore& extents);
   void remove(TupleId tid);

private:
   BufferManager& bufferManager;
   CompactExtentStore& freePages;

   struct PageReference {uint32_t freeBytes; PageId pid;};
   std::vector<PageReference> pages; // Remember each page id and its free bytes

   /// Helper for serialization
   static Record marshall(SegmentId sid, const ExtentStore& extents);
   static std::pair<SegmentId, ExtentStore> unmarshall(const Record& record);
};

}
