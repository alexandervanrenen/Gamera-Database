#include <stdint.h>
#include "gtest/gtest.h"
#include "segment_manager/SlottedPage.hpp"
#include "common/Config.hpp"
#include "util/Utility.hpp"
#include <cstdlib>
#include <unordered_map>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace dbi;

static const uint32_t kTestScale = 1;

TEST(SlottedPage, Simple)
{
    SlottedPage* slottedPage = static_cast<SlottedPage*>(malloc(kPageSize));
    slottedPage->initialize();

    // Insert
    slottedPage->insert(Record("windmill"));
    RecordId fsi = slottedPage->insert(Record("windmill"));
    slottedPage->remove(fsi);

    free(slottedPage);
}

TEST(SlottedPage, SlotReuseAfterDelete)
{
    SlottedPage* slottedPage = static_cast<SlottedPage*>(malloc(kPageSize));
    slottedPage->initialize();

    // Checks if a slot is reused
    RecordId dataRecordId1 = slottedPage->insert(Record("Hello World!"));
    slottedPage->remove(dataRecordId1);
    RecordId dataRecordId2 = slottedPage->insert(Record("Hello World!"));
    ASSERT_EQ(dataRecordId1, dataRecordId2);

    // Even with smaller vales ?
    slottedPage->remove(dataRecordId2);
    RecordId dataRecordId = slottedPage->insert(Record("Hello World"));
    ASSERT_EQ(dataRecordId, dataRecordId2);

    free(slottedPage);
}

TEST(SlottedPage, DefragmentationBasic)
{
    std::string fragmentationData1 = "Hello  !";
    std::string fragmentationData2 = "World  !";
    std::string staticData = "the blob";
    std::string newData = "hello my world !";
    Record fragmentationRecord1(fragmentationData1);
    Record fragmentationRecord2(fragmentationData2);
    Record staticRecord(staticData);
    Record newDataRecord(newData);

    SlottedPage* slottedPage = static_cast<SlottedPage*>(malloc(kPageSize));
    slottedPage->initialize();

    RecordId fragmentationRecordId1 = slottedPage->insert(fragmentationRecord1);    
    RecordId staticRecordId = slottedPage->insert(staticRecord);  
    RecordId fragmentationRecordId2 = slottedPage->insert(fragmentationRecord2);

    // Fill page with ... CRAP!
    while(slottedPage->canHoldRecord(staticRecord))
        slottedPage->insert(Record(staticData));

    // Remove the two values
    ASSERT_TRUE(!slottedPage->canHoldRecord(newDataRecord));
    slottedPage->remove(fragmentationRecordId1);
    slottedPage->remove(fragmentationRecordId2);
    ASSERT_TRUE(slottedPage->getBytesFreeForRecord() >= newData.length());

    // The page should now have a structure like: <freeSpace> <staticData> <freeSpace> (whereby both free space areas are to short for the new record)
    slottedPage->defragment();

    // No side effects on sample record
    ASSERT_EQ(slottedPage->isReference(staticRecordId), kInvalidTupleID);
    ASSERT_EQ(slottedPage->lookup(staticRecordId), Record(staticData));

    // Add now record
    ASSERT_TRUE(slottedPage->getBytesFreeForRecord() >= newData.length());
    RecordId newDataRecordId = slottedPage->insert(newDataRecord);
    ASSERT_EQ(slottedPage->isReference(newDataRecordId), kInvalidTupleID);
    ASSERT_EQ(slottedPage->lookup(newDataRecordId), newDataRecord);

    free(slottedPage);
}

TEST(SlottedPage, ForeignRecords)
{
    SlottedPage* slottedPage = static_cast<SlottedPage*>(malloc(kPageSize));
    slottedPage->initialize();

    // Make foreign record and check
    uint16_t freeBytes = slottedPage->getBytesFreeForRecord();
    RecordId rid = slottedPage->insertForeigner(Record("fear not this night"), 8129);
    ASSERT_EQ(slottedPage->lookup(rid), Record("fear not this night"));
    ASSERT_EQ(slottedPage->isReference(rid), kInvalidTupleID);
    ASSERT_EQ(1u, slottedPage->getAllRecords(0).size());
    ASSERT_EQ(8129ull, slottedPage->getAllRecords(0)[0].first);
    ASSERT_EQ(Record("fear not this night"), slottedPage->getAllRecords(0)[0].second);

    // Update the foreign record
    slottedPage->updateForeigner(rid, 8129, Record("but i am afraid of the dark"));
    ASSERT_EQ(slottedPage->lookup(rid), Record("but i am afraid of the dark"));
    ASSERT_EQ(slottedPage->isReference(rid), kInvalidTupleID);
    ASSERT_EQ(1u, slottedPage->getAllRecords(0).size());
    ASSERT_EQ(8129ull, slottedPage->getAllRecords(0)[0].first);
    ASSERT_EQ(Record("but i am afraid of the dark"), slottedPage->getAllRecords(0)[0].second);

    // Remove foreign record
    slottedPage->remove(rid);
    ASSERT_EQ(slottedPage->getBytesFreeForRecord(), freeBytes);
    ASSERT_EQ(slottedPage->countAllRecords(), 0u);

    free(slottedPage);
}

TEST(SlottedPage, ReferenceRecords)
{
    SlottedPage* slottedPage = static_cast<SlottedPage*>(malloc(kPageSize));
    slottedPage->initialize();

    TId tid = 8129;
    RecordId rid = slottedPage->insert(Record("most awesome paper ever: a system for visualizing human behavior based on car metaphors"));

    // Make reference and check
    slottedPage->updateToReference(rid, tid);
    ASSERT_EQ(tid, slottedPage->isReference(rid));
    ASSERT_EQ(slottedPage->getAllRecords(0).size(), 0u);
    ASSERT_EQ(slottedPage->countAllRecords(), 1u);

    // Remove reference
    slottedPage->remove(rid);
    ASSERT_EQ(slottedPage->getAllRecords(0).size(), 0u);
    ASSERT_EQ(slottedPage->countAllRecords(), 0u);

    free(slottedPage);
}

TEST(SlottedPage, Randomized)
{
    const uint32_t iterations = 10000;

    for(uint32_t j=0; j<kTestScale; j++) {
        std::unordered_map<RecordId, std::string> reference;
        SlottedPage* slottedPage = static_cast<SlottedPage*>(malloc(kPageSize));
        slottedPage->initialize();

        // Add some initial data
        for(uint32_t i=0; i<kPageSize/3/32; i++) {
            std::string data = util::randomWord(8, 64);
            if(slottedPage->getBytesFreeForRecord() < data.size())
                continue;
            RecordId id = slottedPage->insert(Record(data));
            // std::cout << "insert " << id << " -> " << data << std::endl;
            ASSERT_TRUE(reference.count(id) == 0);
            reference.insert(make_pair(id, data));
        }

        // Work on it
        for(uint32_t i=0; i<iterations; i++) {
            int32_t operation = util::ranny() % 100;

            // Do insert
            if(operation <= 40) {
                std::string data = util::randomWord(8, 64);
                if(slottedPage->getBytesFreeForRecord() < data.size())
                    continue;
                RecordId id = slottedPage->insert(Record(data));
                // std::cout << "insert " << id << " -> " << data << std::endl;
                ASSERT_TRUE(reference.count(id) == 0);
                reference.insert(make_pair(id, data));
            }

            // Do remove
            else if(operation <= 80) {
                if(reference.empty())
                    continue;
                RecordId id = reference.begin()->first;
                // std::cout << "remove " << id << std::endl;
                Record record = slottedPage->lookup(id);
                ASSERT_EQ(slottedPage->isReference(id), kInvalidTupleID);
                ASSERT_EQ(std::string(record.data(), record.size()), reference.begin()->second);
                slottedPage->remove(id);
                reference.erase(reference.begin());
            }

            // Do update
            else if(operation <= 98) {
                if(reference.empty())
                    continue;
                RecordId id = reference.begin()->first;
                Record record = slottedPage->lookup(id);
                ASSERT_EQ(slottedPage->isReference(id), kInvalidTupleID);
                ASSERT_EQ(std::string(record.data(), record.size()), reference.begin()->second);
                std::string data = util::randomWord(8, 64);
                if(slottedPage->canUpdateRecord(id, Record(data))) {
                    slottedPage->update(id, Record(data));
                    reference.erase(reference.begin());
                    reference.insert(make_pair(id, data));
                }
            }

            // Do consistency check
            else if(operation<=99 || i==iterations-1 || i==0) {
                ASSERT_TRUE(slottedPage->isValid());
                auto records = slottedPage->getAllRecords(0); // page id does not matter
                ASSERT_EQ(records.size(), reference.size());
                for(auto& iter : records) {
                    ASSERT_TRUE(reference.count(toRecordId(iter.first)) > 0);
                    ASSERT_EQ(string(iter.second.data(), iter.second.size()), reference.find(toRecordId(iter.first))->second);
                }
                continue;
            }
        }
        free(slottedPage);
    }
}
