#include "FunkeSlottedTest.hpp"
#include "util/Utility.hpp"
#include "common/Config.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "segment_manager/SPSegment.hpp"
#include "gtest/gtest.h"
#include "segment_manager/Record.hpp"
#include "operator/TableScanOperator.hpp"
#include <array>
#include <fstream>
#include <string>
#include <set>

using namespace std;
using namespace dbi;

TEST(SegmentManager, Simple)
{
   const string fileName = "swap_file";
   const uint32_t pages = 100;

   ASSERT_TRUE(util::createFile(fileName, pages * kPageSize));
   BufferManager bufferManager(fileName, pages / 2);
   SegmentManager segmentManager(bufferManager, true);

   // Create
   SegmentId id = segmentManager.createSegment(SegmentType::SP, 10);
   SPSegment& segment = segmentManager.getSPSegment(id);

   // Grow
   ASSERT_EQ(segment.getNumPages(), 10ul);
   segmentManager.growSegment(segment, 20ul);
   ASSERT_EQ(segment.getNumPages(), 30ul);

   // Drop
   segmentManager.dropSegment(segment);
   SegmentId id_b = segmentManager.createSegment(SegmentType::SP, 98);
   SPSegment& segment_b = segmentManager.getSPSegment(id_b);
   ASSERT_EQ(segment_b.getNumPages(), 98ul);

   remove(fileName.c_str());
}

// TEST(SegmentManager, SPSegmentSimple)
// {
//    const string fileName = "swap_file";
//    const uint32_t pages = 100;

//    // Create
//    ASSERT_TRUE(util::createFile(fileName, pages * kPageSize));
//    BufferManager bufferManager(fileName, pages / 2);
//    SegmentManager segmentManager(bufferManager, true);
//    SegmentId id = segmentManager.createSegment(SegmentType::SP, 10);
//    SPSegment& segment = segmentManager.getSPSegment(id);

//    // Insert and look up
//    string data = "the clown is down";
//    Record record(data);
//    TId tid = segment.insert(record);
//    ASSERT_EQ(record, segment.lookup(tid));

//    // Update existing page with value not longer than existing
//    string updatedData = "the clown is ?";
//    Record updatedRecord(updatedData);
//    segment.update(tid, updatedRecord);
//    ASSERT_EQ(updatedRecord, segment.lookup(tid));

//    // Update existing page with value longer than existing
//    string longerUpdatedData = "the clown was revived";
//    Record longerUpdatedRecord(longerUpdatedData);
//    segment.update(tid, longerUpdatedRecord);
//    ASSERT_EQ(longerUpdatedRecord, segment.lookup(tid));

//    // TODO: update with value which must be placed on another page

//    // Remove created page
//    segment.remove(tid);

//    remove(fileName.c_str());
// }

// TEST(SegmentManager, FunkeTest)
// {
//    const string fileName = "swap_file";
//    const uint32_t pages = 1 * 1000;

//    ASSERT_TRUE(util::createFile(fileName, pages * kPageSize));
//    ASSERT_EQ(run(fileName, pages), 0);
//    remove(fileName.c_str());
// }
