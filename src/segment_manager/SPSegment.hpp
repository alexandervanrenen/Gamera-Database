#pragma once

#include "Segment.hpp"
#include <vector>

namespace dbi {

class BufferManager;
class Record;
class FSISegment;

class SPSegment : public Segment {
public:
   /// Constructor
   SPSegment(SegmentId id, FSISegment& fsi, SegmentInventory& si, BufferManager& bm);
   virtual ~SPSegment();

   /// Operations on records
   TupleId insert(const Record& record);
   Record lookup(TupleId id);
   void remove(TupleId tId);
   void update(TupleId tId, const Record& record);
   std::vector<std::pair<TupleId, Record>> getAllRecordsOfPage(PageId pageId);

   /// Get extents for this segment (extent is added by the segment inventory)
   virtual const Extent grow();
   virtual const Extent grow(uint64_t numPages);

private:
   FSISegment& freeSpaceInventory;

   /// Looks for a page in this segment large enough to hold length (grows if not found)
   PageId aquirePage(uint16_t length);
   /// Insert record on a new page
   TupleId insertForeigner(TupleId originalTupleId, const Record& record);

   /// Initialize all extents to a slotted page
   void initializeExtent(Extent extent);
};

}
