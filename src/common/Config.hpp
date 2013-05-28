#pragma once

#include "TupleId.hpp"
#include "PageId.hpp"
#include "RecordId.hpp"
#include "SegmentId.hpp"
#include <cstdint>

namespace dbi {

/// The size of a page, has to be a power of two and between 4KB and 32KB
static const uint32_t kPageSize = 1 << 14;

static constexpr TupleId kInvalidTupleID = TupleId(-1);
static constexpr PageId kInvalidPageID = PageId(-1);
static constexpr RecordId kInvalidRecordID = RecordId(-1);

/// Use same id for free space inventory
static const SegmentId kFreeSpaceInventoryId = SegmentId(0);

/// The first page in a linked list like structure of pages used to store the SI.
static const PageId kMetaPageId = PageId(0);
/// The first page which is not used by any database pages (till now only kMetaPage).
static const PageId kFirstFreePageId = PageId(1);

/// The size of the record is limited
/// Minimum: In order to allow for updates it has to be possible to replace any record by a reference.
/// Maximum: Each record has to fit on one page.
static const uint16_t kMinimumRecordSize = 8;
static const uint16_t kMaximumRecordSize = kPageSize - 16 - 4; // 16 == Slotted page header; 4 == slot
}
