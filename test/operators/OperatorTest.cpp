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

TEST(Operator, TableScanEmpty)
{
   const std::string fileName = "swap_file";
   const uint32_t pages = 100;

   // Create
   ASSERT_TRUE(dbi::util::createFile(fileName, pages * dbi::kPageSize));
   dbi::BufferManager bufferManager(fileName, pages / 2);
   dbi::SegmentManager segmentManager(bufferManager, true);
   dbi::SegmentId id = segmentManager.createSegment(dbi::SegmentType::SP, 10);
   dbi::SPSegment& segment = segmentManager.getSPSegment(id);

   // Do scan empty
   dbi::TableScanOperator scanner(segment);
   scanner.open();
   ASSERT_TRUE(!scanner.next());
   scanner.close();

   remove(fileName.c_str());
}

TEST(Operator, TableScan)
{
   const std::string fileName = "swap_file";
   const uint32_t pages = 100;

   // Create
   ASSERT_TRUE(dbi::util::createFile(fileName, pages * dbi::kPageSize));
   dbi::BufferManager bufferManager(fileName, pages / 2);
   dbi::SegmentManager segmentManager(bufferManager, true);
   dbi::SegmentId id = segmentManager.createSegment(dbi::SegmentType::SP, 10);
   dbi::SPSegment& segment = segmentManager.getSPSegment(id);

   // Insert some values
   std::set<dbi::Record> records; // hit chance for each entry 26^32 .. drunken alex says: "lets risk it :D"
   for(uint32_t i = 0; i < 100; i++) {
      std::string data = dbi::util::randomWord(32);
      records.insert(dbi::Record(data));
      segment.insert(dbi::Record(data));
   }

   // Do scan empty
   dbi::TableScanOperator scanner(segment);
   scanner.open();
   while(scanner.next()) {
      dbi::Record data(scanner.getOutput().data(), scanner.getOutput().size());
      ASSERT_TRUE(records.count(data) > 0);
      records.erase(data);
   }
   scanner.close();

   remove(fileName.c_str());
}
