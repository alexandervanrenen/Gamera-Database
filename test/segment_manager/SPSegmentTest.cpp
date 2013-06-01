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
#include "util/Random.hpp"
#include "segment_manager/SlottedPage.hpp"
#include <array>
#include <fstream>
#include <string>
#include <set>

using namespace std;
using namespace dbi;

TEST(SPSegment, SPSegmentSimple)
{
   const uint32_t pages = 100;
   assert(kSwapFilePages>=pages);

   // Create
   BufferManager bufferManager(kSwapFileName, pages / 2);
   SegmentManager segmentManager(bufferManager, true);
   SegmentId id = segmentManager.createSegment(SegmentType::SP, 10);
   SPSegment& segment = segmentManager.getSPSegment(id);

   // Insert and look up
   string data = "the clown is down";
   Record record(data);
   TupleId tid = segment.insert(record);
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

   // Remove created page
   segment.remove(tid);
}

TEST(SPSegment, SPSegmentManyPageUpdate)
{
   const uint32_t pages = 100;
   assert(kSwapFilePages>=pages);
   const Record smallRecord = Record("the tree of liberty must be refreshed from time to time with the blood of patriots and tyrants. it is it's natural manure.");
   const Record bigRecord1 = Record(string(kPageSize/2, 'a'));
   const Record bigRecord2 = Record(string(kPageSize/2, 'b'));
   const Record bigRecord3 = Record(string(kPageSize/2, 'c'));
   const Record memdiumRecord = Record(string(kPageSize/4, 'd'));

   // Create
   BufferManager bufferManager(kSwapFileName, pages / 2);
   SegmentManager segmentManager(bufferManager, true);
   SegmentId id = segmentManager.createSegment(SegmentType::SP, 10);
   SPSegment& segment = segmentManager.getSPSegment(id);

   // Trigger local update: p1:[1=small, 2=big] p2:[] p3:[]
   TupleId tid1 = segment.insert(smallRecord);
   TupleId tid2 = segment.insert(smallRecord);
   segment.update(tid2, bigRecord2);

   // Trigger a non page local update: p1:[2=big] p2:[1=big] p3:[]
   segment.update(tid1, bigRecord1);
   ASSERT_EQ(bigRecord1, segment.lookup(tid1));
   ASSERT_EQ(bigRecord2, segment.lookup(tid2));

   // Trigger update on reference value: p1:[2=big] p2:[1=big] p3:[]
   segment.update(tid1, bigRecord3);
   ASSERT_EQ(bigRecord3, segment.lookup(tid1));
   ASSERT_EQ(bigRecord2, segment.lookup(tid2));

   // Trigger update on reference value with overflow: p1:[2=big, 3=medium] p2:[1=big] p3:[4=big]
   TupleId tid3 = segment.insert(memdiumRecord);
   TupleId tid4 = segment.insert(smallRecord);
   segment.update(tid4, memdiumRecord);
   segment.update(tid4, bigRecord3);
   ASSERT_EQ(bigRecord3, segment.lookup(tid4));

   // Trigger collapse reference: p1:[2=big, 3=medium, 4=small] p2:[1=big] p3:[]
   segment.update(tid4, smallRecord);
   ASSERT_EQ(bigRecord3, segment.lookup(tid1));
   ASSERT_EQ(bigRecord2, segment.lookup(tid2));
   ASSERT_EQ(memdiumRecord, segment.lookup(tid3));
   ASSERT_EQ(smallRecord, segment.lookup(tid4));

   // Remove everything
   segment.remove(tid1);
   segment.remove(tid2);
   segment.remove(tid3);
   segment.remove(tid4);

   // Check that page is empty
   TableScanOperator scanner(segment);
   scanner.open();
   ASSERT_TRUE(!scanner.next());
   scanner.close();
}

TEST(SPSegment, Randomized)
{
   const uint32_t kTestScale = 1;
   const uint32_t kIterations = 10000;
   const uint32_t kPages = 1000;
   assert(kSwapFilePages>=kPages);
   const uint32_t kMaxWordSize = 512;
   util::Random ranny;

   for(uint32_t j=0; j<kTestScale; j++) {
      /// Create structure
      BufferManager bufferManager(kSwapFileName, kPages / 2);
      auto segmentManager = util::make_unique<SegmentManager>(bufferManager, true);
      SegmentId id = segmentManager->createSegment(SegmentType::SP, 10);
      SPSegment* segment = &segmentManager->getSPSegment(id);
      unordered_map<TupleId, string> reference;
      uint32_t insertedSize = 0;
      uint32_t removedSize = 0;

      // Add some initial data
      for(uint32_t i=0; i<kPageSize/3/32; i++) {
         string data = util::randomWord(ranny, 8, kMaxWordSize);
         insertedSize += data.size();
         TupleId id = segment->insert(Record(data));
         ASSERT_TRUE(reference.count(id) == 0);
         reference.insert(make_pair(id, data));
         // cout << "initial insert " << id << " -> " << data << endl;
      }

     // Work on it
     for(uint32_t i=0; i<kIterations; i++) {
         int32_t operation = ranny.rand() % 100;

         // Do insert
         if(operation <= 40) {
            string data = util::randomWord(ranny, kMaxWordSize, 4*kMaxWordSize);
            insertedSize += data.size();
            TupleId id = segment->insert(Record(data));
            ASSERT_TRUE(reference.count(id) == 0);
            reference.insert(make_pair(id, data));
            // cout << "insert " << id << " -> " << data << endl;
         }

         // Do remove
         else if(operation <= 60) {
            if(reference.empty())
               continue;
            auto iter = reference.begin();
            advance(iter, ranny.rand()%reference.size());
            TupleId id = iter->first;
            Record record = segment->lookup(id);
            removedSize += record.size();
            ASSERT_EQ(string(record.data(), record.size()), iter->second);
            segment->remove(id);
            reference.erase(iter);
            // cout << "remove " << id << endl;
         }

         // Do update
         else if(operation <= 97) {
            if(reference.empty())
               continue;
            auto iter = reference.begin();
            advance(iter, ranny.rand()%reference.size());
            TupleId id = iter->first;
            ASSERT_EQ(string(segment->lookup(id).data(), segment->lookup(id).size()), iter->second);
            string data = util::randomWord(ranny, 8, kMaxWordSize);
            segment->update(id, Record(data));
            ASSERT_EQ(string(segment->lookup(id).data(), segment->lookup(id).size()), data);
            reference.erase(iter);
            reference.insert(make_pair(id, data));
            // cout << "update " << id << " to " << data << endl;
         }

         // Do restart of the database
         else if(operation<=98) {
            segmentManager = util::make_unique<SegmentManager>(bufferManager, false);
            segment = &segmentManager->getSPSegment(id);
         }

         // Do consistency check
         else if(operation<=99 || i==kIterations-1 || i==0) {
            // Do scan empty
            dbi::TableScanOperator scanner(*segment);
            scanner.open();
            while(scanner.next()) {
               const std::pair<dbi::TupleId, dbi::Record>& record = scanner.getOutput();
               ASSERT_TRUE(reference.count(record.first) > 0);
               ASSERT_EQ(string(record.second.data(), record.second.size()), reference.find(record.first)->second);
            }
            scanner.close();
         }
      }

      uint64_t sum = 0;
      for(auto iter=segment->beginPageId(); iter!=segment->endPageId(); iter++) {
         auto& bf = bufferManager.fixPage(*iter, kShared);
         auto& sp = reinterpret_cast<SlottedPage&>(*bf.data());
         sum += sp.getBytesFreeForRecord();
         bufferManager.unfixPage(bf, kClean);
      }
      // cout << "pages: " << segment->numPages() << endl;
      // cout << "sum " << sum << endl;
      // cout << "insert " << insertedSize << endl;
      // cout << "remove " << removedSize << endl;
   }
}
