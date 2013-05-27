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

   segmentManager.growSegment(segment, 10ul);
   ASSERT_EQ(segment.getNumPages(), 40ul);

   // Drop
   segmentManager.dropSegment(segment);
   SegmentId id_b = segmentManager.createSegment(SegmentType::SP, 98);
   SPSegment& segment_b = segmentManager.getSPSegment(id_b);
   ASSERT_EQ(segment_b.getNumPages(), 98ul);

   remove(fileName.c_str());
}

TEST(SegmentManager, PersistentSISingle)
{
   const string fileName = "swap_file";
   const uint32_t pages = 100;

   ASSERT_TRUE(util::createFile(fileName, pages * kPageSize));
   SegmentId sid1;
   TId tid;

   // Create
   {
      // Add one 10 page segment
      BufferManager bufferManager(fileName, pages / 2);
      SegmentManager segmentManager(bufferManager, true);
      sid1 = segmentManager.createSegment(SegmentType::SP, 10);
      SPSegment& segment1 = segmentManager.getSPSegment(sid1);
      tid = segment1.insert(Record("Experience is simply the name we give our mistakes - Oscar Wilde"));
   }

   // Restart
   {
      // Check that the 10 page segment is still there
      BufferManager bufferManager(fileName, pages / 2);
      SegmentManager segmentManager(bufferManager, false);
      SPSegment& segment1 = segmentManager.getSPSegment(sid1);
      ASSERT_EQ(segment1.lookup(tid), Record("Experience is simply the name we give our mistakes - Oscar Wilde"));
   }

   remove(fileName.c_str());
}

TEST(SegmentManager, PersistentSIList)
{
   const string fileName = "swap_file";
   const uint32_t pages = 4000;

   ASSERT_TRUE(util::createFile(fileName, pages * kPageSize));
   SegmentId sid1;
   SegmentId sid2;
   SegmentId sid3;

   // Create
   {
      BufferManager bufferManager(fileName, pages / 2);
      SegmentManager segmentManager(bufferManager, true);
      sid1 = segmentManager.createSegment(SegmentType::SP, 1);
      SPSegment& segment1 = segmentManager.getSPSegment(sid1);
      sid2 = segmentManager.createSegment(SegmentType::SP, 1);
      SPSegment& segment2 = segmentManager.getSPSegment(sid2);
      sid3 = segmentManager.createSegment(SegmentType::SP, 1);
      SPSegment& segment3 = segmentManager.getSPSegment(sid3);

      for(uint32_t i=0; i<kPageSize/16 - 12; i++) {
         segmentManager.growSegment(segment1, 1);
         segmentManager.growSegment(segment2, 1);
         segmentManager.growSegment(segment3, 1);
      }

      ASSERT_EQ(segment1.getNumPages(), kPageSize/16 - 11);
      ASSERT_EQ(segment2.getNumPages(), kPageSize/16 - 11);
      ASSERT_EQ(segment3.getNumPages(), kPageSize/16 - 11);
   }

   // Restart
   {
      BufferManager bufferManager(fileName, pages / 2);
      SegmentManager segmentManager(bufferManager, false);
      SPSegment& segment1 = segmentManager.getSPSegment(sid1);
      SPSegment& segment2 = segmentManager.getSPSegment(sid2);
      SPSegment& segment3 = segmentManager.getSPSegment(sid3);

      ASSERT_EQ(segment1.getNumPages(), kPageSize/16 - 11);
      ASSERT_EQ(segment2.getNumPages(), kPageSize/16 - 11);
      ASSERT_EQ(segment3.getNumPages(), kPageSize/16 - 11);
   }

   remove(fileName.c_str());
}

TEST(SegmentManager, SPSegmentSimple)
{
   const string fileName = "swap_file";
   const uint32_t pages = 100;

   // Create
   ASSERT_TRUE(util::createFile(fileName, pages * kPageSize));
   BufferManager bufferManager(fileName, pages / 2);
   SegmentManager segmentManager(bufferManager, true);
   SegmentId id = segmentManager.createSegment(SegmentType::SP, 10);
   SPSegment& segment = segmentManager.getSPSegment(id);

   // Insert and look up
   string data = "the clown is down";
   Record record(data);
   TId tid = segment.insert(record);
   ASSERT_EQ(record, segment.lookup(tid));

   // Update existing page with value not longer than existing
   string updatedData = "the clown is ?";
   Record updatedRecord(updatedData);
   segment.update(tid, updatedRecord);
   ASSERT_EQ(updatedRecord, segment.lookup(tid));

   // Update existing page with value longer than existing
   string longerUpdatedData = "the clown was revived";
   Record longerUpdatedRecord(longerUpdatedData);
   segment.update(tid, longerUpdatedRecord);
   ASSERT_EQ(longerUpdatedRecord, segment.lookup(tid));

   // TODO: update with value which must be placed on another page

   // Remove created page
   segment.remove(tid);

   remove(fileName.c_str());
}

TEST(SegmentManager, SPSegmentManyPageUpdate)
{
   const string fileName = "swap_file";
   const uint32_t pages = 100;
   const Record smallRecord = Record("the tree of liberty must be refreshed from time to time with the blood of patriots and tyrants. it is it's natural manure.");
   const Record bigRecord1 = Record(string(kPageSize/2, 'a'));
   const Record bigRecord2 = Record(string(kPageSize/2, 'b'));

   // Create
   ASSERT_TRUE(util::createFile(fileName, pages * kPageSize));
   BufferManager bufferManager(fileName, pages / 2);
   SegmentManager segmentManager(bufferManager, true);
   SegmentId id = segmentManager.createSegment(SegmentType::SP, 10);
   SPSegment& segment = segmentManager.getSPSegment(id);

   // Establish structure: <12 Byte> <kPageSize/2>
   TId tid1 = segment.insert(smallRecord);
   TId tid2 = segment.insert(bigRecord2);

   // Now trigger a non page local update by increasing the 12 Byte to kPageSize/2 byte
   segment.update(tid1, bigRecord1);

   // Check result
   ASSERT_EQ(bigRecord1, segment.lookup(tid1));
   ASSERT_EQ(bigRecord2, segment.lookup(tid2));
   TableScanOperator scanner(segment);
   scanner.open();
   while(scanner.next()) {
      const pair<TId, Record>& record = scanner.getOutput();
      ASSERT_TRUE(record.first == tid1 || record.first == tid2);
      if(record.first == tid1)
         ASSERT_TRUE(record.second == bigRecord1); else
         ASSERT_TRUE(record.second == bigRecord2);
   }
   scanner.close();

   // Update back to smaller record


   // Remove both records
   // segment.remove(tid1);
   // segment.remove(tid2);
}

// TEST(SegmentManager, FunkeTest)
// {
//    const string fileName = "swap_file";
//    const uint32_t pages = 1 * 1000;

//    ASSERT_TRUE(util::createFile(fileName, pages * kPageSize));
//    ASSERT_EQ(run(fileName, pages), 0);
//    remove(fileName.c_str());
// }
