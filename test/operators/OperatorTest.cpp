#include "test/TestConfig.hpp"
#include "util/Utility.hpp"
#include "common/Config.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "segment_manager/SPSegment.hpp"
#include "gtest/gtest.h"
#include "segment_manager/Record.hpp"
#include "operator/TableScanOperator.hpp"
#include "util/Random.hpp"
#include <array>
#include <fstream>
#include <string>
#include <unordered_map>

TEST(Operator, TableScanEmpty)
{
   const uint32_t pages = 100;
   assert(kSwapFilePages>=pages);

   // Create
   dbi::BufferManager bufferManager(kSwapFileName, pages / 2);
   dbi::SegmentManager segmentManager(bufferManager, true);
   dbi::SegmentId id = segmentManager.createSegment(dbi::SegmentType::SP, 10);
   dbi::SPSegment& segment = segmentManager.getSPSegment(id);

   // Do scan empty
   dbi::TableScanOperator scanner(segment);
   scanner.open();
   ASSERT_TRUE(!scanner.next());
   scanner.close();
}

TEST(Operator, TableScan)
{
   const uint32_t pages = 100;
   assert(kSwapFilePages >= pages);
   dbi::util::Random ranny;

   // Create
   dbi::BufferManager bufferManager(kSwapFileName, pages / 2);
   dbi::SegmentManager segmentManager(bufferManager, true);
   dbi::SegmentId id = segmentManager.createSegment(dbi::SegmentType::SP, 10);
   dbi::SPSegment& segment = segmentManager.getSPSegment(id);

   // Insert some values
   std::unordered_map<dbi::TupleId, dbi::Record> records; // hit chance for each entry 26^32 .. drunken alex says: "lets risk it :D"
   for(uint32_t i = 0; i < 100; i++) {
      std::string data = dbi::util::randomWord(ranny, 8, 64);
      dbi::TupleId id = segment.insert(dbi::Record(data));
      records.insert(std::make_pair(id, dbi::Record(data)));
   }

   // Do scan empty
   dbi::TableScanOperator scanner(segment);
   scanner.open();
   while(scanner.next()) {
      const std::pair<dbi::TupleId, dbi::Record>& record = scanner.getOutput();
      ASSERT_TRUE(records.count(record.first) > 0);
      ASSERT_TRUE(record.second == records.find(record.first)->second);
      records.erase(record.first);
   }
   scanner.close();
}


   // TableScanOperator scanner(segment);
   // scanner.open();
   // while(scanner.next()) {
   //    const pair<TupleId, Record>& record = scanner.getOutput();
   //    ASSERT_TRUE(record.first == tid1 || record.first == tid2);
   //    if(record.first == tid1)
   //       ASSERT_TRUE(record.second == bigRecord3); else
   //       ASSERT_TRUE(record.second == bigRecord2);
   // }
   // scanner.close();