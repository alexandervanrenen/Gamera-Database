#pragma once

#include "common/Config.hpp"
#include <unordered_map>
#include <vector>

namespace dbi {

class BufferManager;
class ExtentStore;

class Persister {
public:
   Persister(BufferManager& bufferManager, ExtentStore& freePages);
   void create(std::unordered_map<SegmentId, std::pair<TId, ExtentStore>>& segmentMap);
   void load(std::unordered_map<SegmentId, std::pair<TId, ExtentStore>>& segmentMap);

   TId insert(SegmentId sid, const ExtentStore& extents);
   TId update(TId tid, SegmentId sid, const ExtentStore& extents);
   void remove(TId tid);

private:
   BufferManager& bufferManager;
   ExtentStore& freePages;
   std::vector<PageId> pages;
};

}
