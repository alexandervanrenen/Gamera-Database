#pragma once

#include <cstdint>

namespace dbi {

using TId = uint64_t;
// 16 Bit most significant bits are used for record id
using PageId = uint64_t;
// 16 Bit most significant bits should be 0 otherwise page id is invalid
using SegmentId = uint64_t;
using RecordId = uint16_t;

inline PageId toPageId(TId id)
{
   return id >> 16;
}
inline RecordId toRecordId(TId id)
{
   return id & 0xffff;
}
inline TId toTID(PageId pid, RecordId rId)
{
   return (pid << 16) + rId;
}

static const uint32_t kPageSize = 1 << 14; // less than 1<<14 .. otherwise slotted page breaks (terribly(!))

static const PageId kInvalidPageID = -1;
static const RecordId kInvalidRecordID = -1;
}
