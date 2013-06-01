#include <gtest/gtest.h>
#include <string>
#include <cstdint>
#include <cassert>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <thread>

#include "util/Utility.hpp"
#include "common/Config.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "schema/MetadataManager.hpp"
#include "sqlparser/Parser.hpp"
#include "sqlparser/Schema.hpp"




TEST(MetadataManagerTest, SimpleTest) {
    typedef dbi::SegmentId SegmentId;
    const std::string fileName = "bin/swap_file";
    const std::string schemaFile = "test/schema/test.sql";
    const uint32_t pages = 100;

    // Create
    ASSERT_TRUE(dbi::util::createFile(fileName, pages * dbi::kPageSize));
    {
        dbi::BufferManager bufferManager(fileName, pages / 2);
        dbi::SegmentManager segmentManager(bufferManager, true);
        dbi::MetadataManager mdm { segmentManager };
        dbi::sqlparser::Parser p(schemaFile);
        std::unique_ptr<dbi::sqlparser::Schema> schema = p.parse();
        for (auto r: schema.get()->relations) {
            mdm.addRelation(r);
        }
        mdm.setSegment("country", SegmentId(5));
        mdm.setSegment("employee", SegmentId(6));
        //mdm.addIndex("employee", SegmentId(7), {"id", "country_id"});
        ASSERT_EQ(mdm.getSegmentForRelation("country"), SegmentId(5));
        ASSERT_EQ(mdm.getSegmentForRelation("employee"), SegmentId(6));
    }
    // Load data from disk to see it persisting works
    {
        dbi::BufferManager bufferManager(fileName, pages / 2);
        dbi::SegmentManager segmentManager(bufferManager, false);
        dbi::MetadataManager mdm { segmentManager };
        ASSERT_EQ(mdm.getSegmentForRelation("country"), SegmentId(5));
        ASSERT_EQ(mdm.getSegmentForRelation("employee"), SegmentId(6));
    }
}



