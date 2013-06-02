#include <gtest/gtest.h>
#include <string>
#include <cstdint>
#include <cassert>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <thread>

#include "test/TestConfig.hpp"
#include "util/Utility.hpp"
#include "common/Config.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "schema/MetadataManager.hpp"
#include "schema/Schema.hpp"
#include "sqlparser/Parser.hpp"
#include "sqlparser/Schema.hpp"




TEST(MetadataManagerTest, SimpleTest) {
    typedef dbi::SegmentId SegmentId;
    const std::string schemaFile = "test/schema/test.sql";
    const uint32_t pages = 100;
    assert(kSwapFilePages>=pages);

    // Create
    {
        dbi::BufferManager bufferManager(kSwapFileName, pages / 2);
        dbi::SegmentManager segmentManager(bufferManager, true);
        dbi::MetadataManager mdm { segmentManager };
        dbi::sqlparser::Parser p(schemaFile);
        std::unique_ptr<dbi::sqlparser::Schema> schema = p.parse();
        for (auto r: schema.get()->relations) {
            mdm.addRelation(r);
        }
        mdm.setSegment("country", SegmentId(5));
        mdm.setSegment("employee", SegmentId(6));
        mdm.addIndex("employee", SegmentId(7), {"id", "country_id"});
        mdm.addIndex("employee", SegmentId(8), {"id"});
        ASSERT_EQ(mdm.getSegmentForRelation("country"), SegmentId(5));
        ASSERT_EQ(mdm.getSegmentForRelation("employee"), SegmentId(6));
        ASSERT_EQ(dbi::AttributeType::Integer, mdm.getTypeForAttribute("employee", "id"));
        ASSERT_EQ(dbi::AttributeType::Char, mdm.getTypeForAttribute("employee", "last_name"));
        // Check that indexes were correctly saved
        dbi::MetadataManager::RelationIndexes idxs = mdm.getRelationIndexes("employee");
        ASSERT_EQ(idxs.size(), uint64_t(2));
        for (dbi::IndexMetadata* idx : idxs) {
            if (idx->attributes.size() == uint64_t(2)) {
                ASSERT_EQ(idx->segment, SegmentId(7));
                for (dbi::AttributeMetadata* am : idx->attributes)
                    ASSERT_TRUE(am->name == "id" || am->name == "country_id");
            } else if (idx->attributes.size() == uint64_t(1)) {
                ASSERT_EQ(idx->segment, SegmentId(8));
                ASSERT_EQ(idx->attributes[0]->name, "id");
            } else 
                ASSERT_TRUE(false);
        }
        // Check that attribute reordering is correct
        ASSERT_EQ(0, mdm.getAttributeOffset("employee", "id")); 
        ASSERT_EQ(8, mdm.getAttributeOffset("employee", "mgr_id")); 
        ASSERT_EQ(16, mdm.getAttributeOffset("employee", "salery")); 
        ASSERT_EQ(24, mdm.getAttributeOffset("employee", "last_name")); 
        ASSERT_EQ(49, mdm.getAttributeOffset("employee", "first_name")); 
        ASSERT_EQ(69, mdm.getAttributeOffset("employee", "country_id")); 
        ASSERT_EQ(71, mdm.getAttributeOffset("employee", "middle")); 
    }
    // Load data from disk to see it persisting works
    {
        dbi::BufferManager bufferManager(kSwapFileName, pages / 2);
        dbi::SegmentManager segmentManager(bufferManager, false);
        dbi::MetadataManager mdm { segmentManager };
        ASSERT_EQ(mdm.getSegmentForRelation("country"), SegmentId(5));
        ASSERT_EQ(mdm.getSegmentForRelation("employee"), SegmentId(6));
        ASSERT_EQ(dbi::AttributeType::Integer, mdm.getTypeForAttribute("employee", "id"));
        ASSERT_EQ(dbi::AttributeType::Char, mdm.getTypeForAttribute("employee", "last_name"));
        // Check that indexes were correctly persisted 
        dbi::MetadataManager::RelationIndexes idxs = mdm.getRelationIndexes("employee");
        ASSERT_EQ(idxs.size(), uint64_t(2));
        for (dbi::IndexMetadata* idx : idxs) {
            if (idx->attributes.size() == uint64_t(2)) {
                ASSERT_EQ(idx->segment, SegmentId(7));
                for (dbi::AttributeMetadata* am : idx->attributes)
                    ASSERT_TRUE(am->name == "id" || am->name == "country_id");
            } else if (idx->attributes.size() == uint64_t(1)) {
                ASSERT_EQ(idx->segment, SegmentId(8));
                ASSERT_EQ(idx->attributes[0]->name, "id");
            } else 
                ASSERT_TRUE(false);
        } 
        // Check that attribute reordering was correctly persisted
        ASSERT_EQ(0, mdm.getAttributeOffset("employee", "id")); 
        ASSERT_EQ(8, mdm.getAttributeOffset("employee", "mgr_id")); 
        ASSERT_EQ(16, mdm.getAttributeOffset("employee", "salery")); 
        ASSERT_EQ(24, mdm.getAttributeOffset("employee", "last_name")); 
        ASSERT_EQ(49, mdm.getAttributeOffset("employee", "first_name")); 
        ASSERT_EQ(69, mdm.getAttributeOffset("employee", "country_id")); 
        ASSERT_EQ(71, mdm.getAttributeOffset("employee", "middle")); 
    }
}



