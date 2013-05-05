#pragma once

#include "FunkeSlottedTest.hpp"
#include "util/Utility.hpp"
#include "common/Config.hpp"
#include "buffer_manager/BufferManager.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "segment_manager/SPSegment.hpp"
#include "gtest/gtest.h"
#include <array>
#include <fstream>

TEST(SegmentManager, Simple) {

   const std::string fileName = "swap_file";
   const uint32_t pages = 100;

   ASSERT_TRUE(dbiu::createFile(fileName, pages*dbi::kPageSize));
   dbi::BufferManager bufferManager(fileName, pages);
   dbi::SegmentManager segmentManager(bufferManager);

   // Create
   dbi::SegmentID id = segmentManager.createSegment(dbi::SegmentType::SP, 10);
   dbi::SPSegment& segment = segmentManager.getSPSegment(id);

   // Grow
   ASSERT_EQ(segment.getNumPages(), 10ul);
   segmentManager.growSegment(segment, 20ul);
   ASSERT_EQ(segment.getNumPages(), 30ul);

   // Drop
   segmentManager.dropSegment(segment);
   dbi::SegmentID id_b = segmentManager.createSegment(dbi::SegmentType::SP, 99);
   dbi::SPSegment& segment_b = segmentManager.getSPSegment(id_b);
   ASSERT_EQ(segment_b.getNumPages(), 99ul);

   remove(fileName.c_str());
}

TEST(SegmentManager, FunkeTest) {

   const std::string fileName = "swap_file";
   const uint32_t pages = 1*1000;

   ASSERT_TRUE(dbiu::createFile(fileName, pages*dbi::kPageSize));
   ASSERT_EQ(run(fileName, pages), 0);
   remove(fileName.c_str());
}
