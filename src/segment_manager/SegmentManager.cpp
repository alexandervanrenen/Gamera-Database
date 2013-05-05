#include "SegmentManager.hpp"
#include <iostream>

using namespace std;

namespace dbi {
    
SegmentManager::SegmentManager(BufferManager& bufferManager)
: bufferManager(bufferManager)
{
}

SegmentID SegmentManager::createSegment(SegmentType segmentType, uint32_t totalSize)
{
   cout << segmentType << " " << totalSize << endl;
   return 0;
}

Segment& SegmentManager::getSegment(const SegmentID& id)
{
   cout << id << endl;
   return segmentInventory;
}

}
