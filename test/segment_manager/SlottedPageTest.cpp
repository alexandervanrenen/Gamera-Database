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

static const uint32_t kTestScale = 100;

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

// TEST(SlottedPage, DefragmentationBasic)
// {
//     std::string fragmentationData1 = "Hello!";
//     std::string fragmentationData2 = "World!";
//     std::string staticData = "block";
//     std::string newData = "Hello World!";
//     dbi::Record fragmentationRecord1(fragmentationData1);
//     dbi::Record fragmentationRecord2(fragmentationData2);
//     dbi::Record staticRecord(staticData);
//     dbi::Record newDataRecord(newData);
    
//     dbi::SlottedPage* slottedPage = static_cast<dbi::SlottedPage*>(malloc(dbi::kPageSize));
//     slottedPage->initialize();
    
//     dbi::RecordId fragmentationRecordId1 = slottedPage->insert(fragmentationRecord1);    
//     dbi::RecordId staticRecordId = slottedPage->insert(staticRecord);  
//     dbi::RecordId fragmentationRecordId2 = slottedPage->insert(fragmentationRecord2);
    
//     // Fill page with ... CRAP!
//     while(slottedPage->getBytesFreeForRecord() >= staticData.length()){
//         slottedPage->insert(dbi::Record(staticData));
//     }
    
//     uint16_t freeSpaceBeforeNewDataInsert = slottedPage->getBytesFreeForRecord();
//     ASSERT_TRUE(slottedPage->getBytesFreeForRecord() < newData.length());  
//     ASSERT_TRUE(slottedPage->remove(fragmentationRecordId1));
//     ASSERT_TRUE(slottedPage->remove(fragmentationRecordId2));
//     ASSERT_TRUE(slottedPage->getBytesFreeForRecord() >= newData.length());
    
//     // The page should now have a structure like: <freeSpace> <staticData> <freeSpace> (whereby both free space areas are to short for the new record)    
//     slottedPage->defragment();
    
//     // No side effects on sample record
//     ASSERT_EQ(slottedPage->lookup(staticRecordId).data(), staticData);
//     //ASSERT_EQ(slottedPage->getBytesFreeForRecord(), newData.length() + freeSpaceBeforeNewDataInsert);

//     ASSERT_TRUE(slottedPage->getBytesFreeForRecord() >= newData.length());
//     dbi::RecordId newDataRecordId = slottedPage->insert(newDataRecord);
//     ASSERT_EQ(slottedPage->lookup(newDataRecordId).data(), newData);
//     // New record is as long as the two removed ones
//     ASSERT_EQ(freeSpaceBeforeNewDataInsert, slottedPage->getBytesFreeForRecord());
    
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
                dbi::Record record = slottedPage->lookup(id);
                ASSERT_EQ(std::string(record.data(), record.size()), reference.begin()->second);
                slottedPage->remove(id);
                reference.erase(reference.begin());
            }

            // Do update
            else if(operation <= 98) {
                if(reference.empty())
                    continue;
                dbi::RecordId id = reference.begin()->first;
                dbi::Record record = slottedPage->lookup(id);
                ASSERT_EQ(std::string(record.data(), record.size()), reference.begin()->second);
                std::string data = dbi::util::randomWord(random()%64 + 1);
                if(data.size()<=record.size() || data.size()-record.size() <= slottedPage->getBytesFreeForRecord()) {
                    slottedPage->update(id, dbi::Record(data));
                    reference.erase(reference.begin());
                    reference.insert(make_pair(id, data));
                }
            }

            // Do consistency check
            else if(operation<=99 || i==iterations-1 || i==0) {
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
