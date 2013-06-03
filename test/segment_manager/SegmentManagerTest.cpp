#include "FunkeSlottedTest.hpp"
#include "util/Utility.hpp"
#include "common/Config.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "test/TestConfig.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "segment_manager/SPSegment.hpp"
#include "gtest/gtest.h"
#include "segment_manager/Record.hpp"
#include "operator/TableScanOperator.hpp"
#include "segment_manager/ExtentStore.hpp"
#include "util/Random.hpp"
#include <array>
#include <fstream>
#include <string>
#include <set>

using namespace std;
using namespace dbi;

TEST(SegmentManager, Simple)
{
   const uint32_t pages = 100;
   assert(kSwapFilePages>=pages);

   BufferManager bufferManager(kSwapFileName, pages / 2);
   SegmentManager segmentManager(bufferManager, true);

   // Create
   SegmentId id = segmentManager.createSegment(SegmentType::SP, 10);
   SPSegment& segment = segmentManager.getSPSegment(id);

   // Grow
   ASSERT_EQ(segment.numPages(), 10ul);
   segmentManager.growSegment(segment, 20ul);

   segmentManager.growSegment(segment, 10ul);
   ASSERT_EQ(segment.numPages(), 40ul);

   // Drop
   segmentManager.dropSegment(segment);
   SegmentId id_b = segmentManager.createSegment(SegmentType::SP, 98);
   SPSegment& segment_b = segmentManager.getSPSegment(id_b);
   ASSERT_EQ(segment_b.numPages(), 98ul);
}

TEST(SegmentManager, PersistentSISingle)
{
   const uint32_t pages = 100;
   assert(kSwapFilePages>=pages);

   SegmentId sid1;
   TupleId tid;

   // Create
   {
      // Add one 10 page segment
      BufferManager bufferManager(kSwapFileName, pages / 2);
      SegmentManager segmentManager(bufferManager, true);
      sid1 = segmentManager.createSegment(SegmentType::SP, 10);
      SPSegment& segment1 = segmentManager.getSPSegment(sid1);
      tid = segment1.insert(Record("Experience is simply the name we give our mistakes - Oscar Wilde"));
   }

   // Restart
   {
      // Check that the 10 page segment is still there
      BufferManager bufferManager(kSwapFileName, pages / 2);
      SegmentManager segmentManager(bufferManager, false);
      SPSegment& segment1 = segmentManager.getSPSegment(sid1);
      ASSERT_EQ(segment1.lookup(tid), Record("Experience is simply the name we give our mistakes - Oscar Wilde"));
   }
}

TEST(SegmentManager, PersistentSIList)
{
   const uint32_t pages = 4000;
   assert(kSwapFilePages>=pages);

   SegmentId sid1;
   SegmentId sid2;
   SegmentId sid3;

   // Create
   {
      BufferManager bufferManager(kSwapFileName, pages / 2);
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

      ASSERT_EQ(segment1.numPages(), kPageSize/16 - 11);
      ASSERT_EQ(segment2.numPages(), kPageSize/16 - 11);
      ASSERT_EQ(segment3.numPages(), kPageSize/16 - 11);
   }

   // Restart
   {
      BufferManager bufferManager(kSwapFileName, pages / 2);
      SegmentManager segmentManager(bufferManager, false);
      SPSegment& segment1 = segmentManager.getSPSegment(sid1);
      SPSegment& segment2 = segmentManager.getSPSegment(sid2);
      SPSegment& segment3 = segmentManager.getSPSegment(sid3);

      ASSERT_EQ(segment1.numPages(), kPageSize/16 - 11);
      ASSERT_EQ(segment2.numPages(), kPageSize/16 - 11);
      ASSERT_EQ(segment3.numPages(), kPageSize/16 - 11);
   }
}

TEST(SegmentManager, Randomized)
{
   const uint32_t kTestScale = 1;
   const uint32_t kIterations = 10000;
   const uint32_t kPages = 10000;
   assert(kSwapFilePages>=kPages);
   const uint32_t kMaxSegmentSize = 64;
   util::Random ranny;

   for(uint32_t j=0; j<kTestScale; j++) {
      /// Create structure
      BufferManager bufferManager(kSwapFileName, kPages);
      auto segmentManager = util::make_unique<SegmentManager>(bufferManager, true);

      unordered_map<SegmentId, uint32_t> reference;

     // Work on it
     for(uint32_t i=0; i<kIterations; i++) {
         int32_t operation = ranny.rand() % 100;

         // Do insert
         if(operation <= 40) {
            uint32_t pageCount = 1 + ranny.rand() % kMaxSegmentSize;
            SegmentId id = segmentManager->createSegment(SegmentType::SP, pageCount);
            // cout << "insert " << id << " " << pageCount << endl;
            reference.insert({id, pageCount});
         }

         // Do drop
         else if(operation <= 80 && !reference.empty()) {
            auto iter = reference.begin();
            advance(iter, ranny.rand()%reference.size());
            // cout << "drop " << iter->first << endl;
            ASSERT_EQ(segmentManager->getSPSegment(iter->first).numPages(), iter->second);
            segmentManager->dropSegment(segmentManager->getSPSegment(iter->first));
            reference.erase(iter);
         }

         // Do grow
         else if(operation <= 97 && !reference.empty()) {
            // cout << "grow" << endl;
            auto iter = reference.begin();
            advance(iter, ranny.rand()%reference.size());
            SegmentId sid = iter->first;
            ASSERT_EQ(segmentManager->getSPSegment(sid).numPages(), iter->second);
            uint32_t pageCount = 1 + ranny.rand() % kMaxSegmentSize;
            segmentManager->growSegment(segmentManager->getSPSegment(sid), pageCount);
            ASSERT_EQ(segmentManager->getSPSegment(sid).numPages(), iter->second + pageCount);
            reference.erase(iter);
            reference.insert({sid, segmentManager->getSPSegment(sid).numPages()});
         }

         // Do restart of the database
         else if(operation<=98) {
            // cout << "restart" << endl;
            segmentManager = util::make_unique<SegmentManager>(bufferManager, false);
         }

         // Do consistency check
         if(operation<=99 || i==kIterations-1 || i==0) {
            // Do scan empty
            for(auto iter : reference) {
               ASSERT_EQ(segmentManager->getSPSegment(iter.first).numPages(), iter.second);
            }
         }
      }
   }
}
