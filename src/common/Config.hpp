#pragma once

#include <cstdint>

namespace dbi {

   using TId = uint64_t;
   using PageId = uint64_t;
   using SegmentId = uint32_t;
   using RecordId = uint16_t;

   inline PageId toPageId(TId id) {return id>>16;}
   inline RecordId toRecordId(TId id) {return id&0xffff;}
   inline TId toTID(PageId pid, RecordId rId) {return (pid<<16) + rId;}

   static const uint32_t kPageSize = 1<<12; // less than 1<<16 .. otherwise slotted page breaks (terribly(!))

   static const PageId kInvalidPageID = -1;
}
