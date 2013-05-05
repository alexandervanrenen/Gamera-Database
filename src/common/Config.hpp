#pragma once

#include <cstdint>

namespace dbi {

   using TID = uint64_t;
   using PageID = uint64_t;
   using SegmentID = uint32_t;
   using RecordID = uint16_t;

   inline PageID toPageID(TID id) {return id>>16;}
   inline RecordID toRecordID(TID id) {return id&0xffff;}
   inline TID toTID(PageID pid, RecordID rid) {return (pid<<16) + rid;}

   static const uint32_t kPageSize = 1<<12; // less than 1<<16 .. otherwise slotted page breaks (terribly(!))

   static const PageID kInvalidPageID = -1;
}
