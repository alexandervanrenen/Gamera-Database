#pragma once

#include "common/Config.hpp"
#include "SegmentType.hpp"
#include "SISegment.hpp"
#include "FSISegment.hpp"
#include "BTreeSegment.hpp"
#include <memory>
#include <unordered_map>

namespace dbi {

class BufferManager;
class SPSegment;
class BTreeSegment;

class SegmentManager {
public:
   /// Constructor
   SegmentManager(BufferManager& bufferManager, bool isInitialSetup);

   /// Add a new segment with one extent of numPages to the segment inventory
   SegmentId createSegment(SegmentType segmentType, uint32_t numPages);

   /// Add numPages to the already existing segment with the given id
   void growSegment(Segment& id); // Let SegmentManager choose
   void growSegment(Segment& id, uint32_t numPages);

   /// Remove the segment with the given id from the segment inventory
   void dropSegment(Segment& id);

   // Access segment with given id and cast to SPSegment
   SPSegment& getSPSegment(const SegmentId id);
   
   BTreeSegment& getBTreeSegment(const SegmentId id);

   FSISegment& getFSISegment();

private:
   BufferManager& bufferManager;

   SISegment segmentInventory; // What pages belongs to a given segment ?
   std::unique_ptr<FSISegment> freeSpaceInventory; // How full is a given page ?

   std::unordered_map<SegmentId, std::unique_ptr<Segment>> segments; // Buffer segments .. ?
};

}
