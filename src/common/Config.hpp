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

/// The size of a page, has to be a power of two and between 4KB and 32KB
static const uint32_t kPageSize = 1 << 14;

static const TId kInvalidTupleID = -1;
static const PageId kInvalidPageID = -1;
static const RecordId kInvalidRecordID = -1;

/// Use same id for free space inventory
static const SegmentId kFreeSpaceInventoryId = 0;

/// The first page in a linked list like structure of pages used to store the SI.
static const PageId kMetaPageId = 0;
/// The first page which is not used by any database pages (till now only kMetaPage).
static const PageId kFirstFreePageId = 1;

/// The size of the record is limited
/// Minimum: In order to allow for updates it has to be possible to replace any record by a reference.
/// Maximum: Each record has to fit on one page.
static const uint16_t kMinimumRecordSize = 8;
static const uint16_t kMaximumRecordSize = kPageSize - 16 - 4; // 16 == Slotted page header; 4 == slot
}
