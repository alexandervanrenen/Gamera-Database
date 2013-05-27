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

static const uint32_t kTestScale = 1;

TEST(SlottedPage, Simple)
{
    dbi::SlottedPage* slottedPage = static_cast<dbi::SlottedPage*>(malloc(dbi::kPageSize));
    slottedPage->initialize();

    // Insert
    slottedPage->insert(dbi::Record("windmill"));
    dbi::RecordId fsi = slottedPage->insert(dbi::Record("windmill"));
    slottedPage->remove(fsi);

    free(slottedPage);
}

TEST(SlottedPage, SlotReuseAfterDelete)
{
    dbi::SlottedPage* slottedPage = static_cast<dbi::SlottedPage*>(malloc(dbi::kPageSize));
    slottedPage->initialize();

    // Checks if a slot is reused
    dbi::RecordId dataRecordId1 = slottedPage->insert(dbi::Record("Hello World!"));
    slottedPage->remove(dataRecordId1);
    dbi::RecordId dataRecordId2 = slottedPage->insert(dbi::Record("Hello World!"));
    ASSERT_EQ(dataRecordId1, dataRecordId2);

    // Even with smaller vales ?
    slottedPage->remove(dataRecordId2);
    dbi::RecordId dataRecordId = slottedPage->insert(dbi::Record("Hello World"));
    ASSERT_EQ(dataRecordId, dataRecordId2);

    free(slottedPage);
}

TEST(SlottedPage, DefragmentationBasic)
{
    std::string fragmentationData1 = "Hello!";
    std::string fragmentationData2 = "World!";
    std::string staticData = "block";
    std::string newData = "Hello World!";
    dbi::Record fragmentationRecord1(fragmentationData1);
    dbi::Record fragmentationRecord2(fragmentationData2);
    dbi::Record staticRecord(staticData);
    dbi::Record newDataRecord(newData);

    dbi::SlottedPage* slottedPage = static_cast<dbi::SlottedPage*>(malloc(dbi::kPageSize));
    slottedPage->initialize();

    dbi::RecordId fragmentationRecordId1 = slottedPage->insert(fragmentationRecord1);    
    dbi::RecordId staticRecordId = slottedPage->insert(staticRecord);  
    dbi::RecordId fragmentationRecordId2 = slottedPage->insert(fragmentationRecord2);

    // Fill page with ... CRAP!
    while(slottedPage->canHoldRecord(staticRecord))
        slottedPage->insert(dbi::Record(staticData));

    // Remove the two values
    ASSERT_TRUE(!slottedPage->canHoldRecord(newDataRecord));
    slottedPage->remove(fragmentationRecordId1);
    slottedPage->remove(fragmentationRecordId2);
    ASSERT_TRUE(slottedPage->getBytesFreeForRecord() >= newData.length());

    // The page should now have a structure like: <freeSpace> <staticData> <freeSpace> (whereby both free space areas are to short for the new record)
    slottedPage->defragment();

    // No side effects on sample record
    ASSERT_EQ(slottedPage->lookup(staticRecordId).first, dbi::kInvalidTupleID);
    ASSERT_EQ(slottedPage->lookup(staticRecordId).second.data(), staticData);

    // Add now record
    ASSERT_TRUE(slottedPage->getBytesFreeForRecord() >= newData.length());
    dbi::RecordId newDataRecordId = slottedPage->insert(newDataRecord);
    ASSERT_EQ(slottedPage->lookup(newDataRecordId).first, dbi::kInvalidTupleID);
    ASSERT_EQ(slottedPage->lookup(newDataRecordId).second, newDataRecord);

    free(slottedPage);
}

TEST(SlottedPage, ForeignRecords)
{
    dbi::SlottedPage* slottedPage = static_cast<dbi::SlottedPage*>(malloc(dbi::kPageSize));
    slottedPage->initialize();

    uint16_t freeBytes = slottedPage->getBytesFreeForRecord();
    dbi::RecordId rid = slottedPage->insertForeigner(dbi::Record("fear not this night"), 8129);
    ASSERT_EQ(dbi::Record("fear not this night"), slottedPage->lookup(rid).second);
    ASSERT_EQ(dbi::kInvalidTupleID, slottedPage->lookup(rid).first);
    ASSERT_EQ(1u, slottedPage->getAllRecords(0).size());
    ASSERT_EQ(8129ull, slottedPage->getAllRecords(0)[0].first);
    ASSERT_EQ(dbi::Record("fear not this night"), slottedPage->getAllRecords(0)[0].second);

    slottedPage->remove(rid);
    ASSERT_EQ(slottedPage->getBytesFreeForRecord(), freeBytes);
    ASSERT_EQ(slottedPage->countAllRecords(), 0u);

    free(slottedPage);
}

// TEST(SlottedPage, ReferenceRecords)
// {
//     dbi::SlottedPage* slottedPage = static_cast<dbi::SlottedPage*>(malloc(dbi::kPageSize));
//     slottedPage->initialize();

//     dbi::TId tid = 8129;
//     dbi::RecordId rid = slottedPage->insert(dbi::Record("6=3!"));
//     slottedPage->updateToReference(rid, tid);
//     ASSERT_EQ(dbi::Record(reinterpret_cast<char*>(&tid), sizeof(dbi::TId)), slottedPage->lookup(rid).second);
//     ASSERT_EQ(tid, slottedPage->lookup(rid).first);
//     slottedPage->remove(rid);
//     ASSERT_EQ(slottedPage->countAllRecords(), 0u);

//     free(slottedPage);
// }

TEST(SlottedPage, Randomized)
{
    const uint32_t iterations = 10000;

    for(uint32_t j=0; j<kTestScale; j++) {
        std::unordered_map<dbi::RecordId, std::string> reference;
        dbi::SlottedPage* slottedPage = static_cast<dbi::SlottedPage*>(malloc(dbi::kPageSize));
        slottedPage->initialize();

        // Add some initial data
        for(uint32_t i=0; i<dbi::kPageSize/3/32; i++) {
            std::string data = dbi::util::randomWord(random()%64 + 1);
            if(slottedPage->getBytesFreeForRecord() < data.size())
                continue;
            dbi::RecordId id = slottedPage->insert(dbi::Record(data));
            // std::cout << "insert " << id << " -> " << data << std::endl;
            ASSERT_TRUE(reference.count(id) == 0);
            reference.insert(make_pair(id, data));
        }

        // Work on it
        for(uint32_t i=0; i<iterations; i++) {
            int32_t operation = random() % 100;

            // Do insert
            if(operation <= 40) {
                std::string data = dbi::util::randomWord(random()%64 + 1);
                if(slottedPage->getBytesFreeForRecord() < data.size())
                    continue;
                dbi::RecordId id = slottedPage->insert(dbi::Record(data));
                // std::cout << "insert " << id << " -> " << data << std::endl;
                ASSERT_TRUE(reference.count(id) == 0);
                reference.insert(make_pair(id, data));
            }

            // Do remove
            else if(operation <= 80) {
                if(reference.empty())
                    continue;
                dbi::RecordId id = reference.begin()->first;
                // std::cout << "remove " << id << std::endl;
                pair<dbi::TId, dbi::Record> record = slottedPage->lookup(id);
                ASSERT_EQ(record.first, dbi::kInvalidTupleID);
                ASSERT_EQ(std::string(record.second.data(), record.second.size()), reference.begin()->second);
                slottedPage->remove(id);
                reference.erase(reference.begin());
            }

            // Do update
            else if(operation <= 98) {
                if(reference.empty())
                    continue;
                dbi::RecordId id = reference.begin()->first;
                pair<dbi::TId, dbi::Record> record = slottedPage->lookup(id);
                ASSERT_EQ(record.first, dbi::kInvalidTupleID);
                ASSERT_EQ(std::string(record.second.data(), record.second.size()), reference.begin()->second);
                std::string data = dbi::util::randomWord(random()%64 + 1);
                if(slottedPage->canUpdateRecord(id, dbi::Record(data))) {
                    slottedPage->update(id, dbi::Record(data));
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
                    ASSERT_TRUE(reference.count(dbi::toRecordId(iter.first)) > 0);
                    ASSERT_EQ(string(iter.second.data(), iter.second.size()), reference.find(dbi::toRecordId(iter.first))->second);
                }
                continue;
            }
        }
        free(slottedPage);
    }
}
