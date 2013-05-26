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

TEST(SlottedPage, Simple)
{
    dbi::SlottedPage* slottedPage = static_cast<dbi::SlottedPage*>(malloc(dbi::kPageSize));
    slottedPage->initialize();

    // Insert
    dbi::RecordId master = slottedPage->insert(dbi::Record("windmill")); // 8 byte
    dbi::RecordId fsi = slottedPage->insert(dbi::Record("windmill")); // 8 byte
    slottedPage->remove(fsi);
    fsi = slottedPage->insert(dbi::Record("windmillwindmillwindmill")); // 24 byte

    dbi::RecordId first = slottedPage->insert(dbi::Record("windmill")); // 8 byte
    slottedPage->remove(first);
    first = slottedPage->insert(dbi::Record("windmillwindmillwindmill")); // 8 byte
    slottedPage->remove(first);
    first = slottedPage->insert(dbi::Record("windmillwindmillwindmill")); // 8 byte
    slottedPage->remove(first);
    first = slottedPage->insert(dbi::Record("windmillwindmillwindmill")); // 8 byte

    free(slottedPage);
}

TEST(SlottedPage, SlotReuseAfterDelete)
{
    dbi::SlottedPage* slottedPage = static_cast<dbi::SlottedPage*>(malloc(dbi::kPageSize));
    slottedPage->initialize();

    // Checks if a slot is reused
    dbi::RecordId dataRecordId1 = slottedPage->insert(dbi::Record("Hello World!"));
    ASSERT_TRUE(slottedPage->remove(dataRecordId1));
    dbi::RecordId dataRecordId2 = slottedPage->insert(dbi::Record("Hello World!"));
    ASSERT_EQ(dataRecordId1, dataRecordId2);

    // Even with smaller vales ?
    ASSERT_TRUE(slottedPage->remove(dataRecordId2));
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

// TEST(SlottedPage, Randomized)
// {
//     std::unordered_map<dbi::RecordId, std::string> reference;
//     dbi::SlottedPage* slottedPage = static_cast<dbi::SlottedPage*>(malloc(dbi::kPageSize));
//     slottedPage->initialize();

//     for(uint32_t i=0; i<10000; i++) {
//         int32_t operation = random() % 100;
//         // std::cout << reference.size() << std::endl;
//         // std::cout << slottedPage->getAllRecords(0).size() << std::endl;

//         // Do insert
//         if(operation <= 40) {
//             std::string data = dbi::util::randomWord(random()%64);
//             if(slottedPage->getBytesFreeForRecord() < data.size())
//                 continue;
//             dbi::RecordId id = slottedPage->insert(dbi::Record(data));
//             // std::cout << "insert " << id << std::endl;
//             ASSERT_TRUE(reference.count(id) == 0);
//             reference.insert(make_pair(id, data));
//         }

//         // Do remove
//         else if(operation <= 60) {
//             if(reference.empty())
//                 continue;
//             dbi::RecordId id = reference.begin()->first;
//             // std::cout << "remove " << id << std::endl;
//             dbi::Record record = slottedPage->lookup(id);
//             ASSERT_EQ(std::string(record.data(), record.size()), reference.begin()->second);
//             slottedPage->remove(id);
//             reference.erase(reference.begin());
//         }

//         // // Do update
//         // else if(operation >= 98) {
//         //     if(reference.empty())
//         //         continue;
//         //     dbi::RecordId id = reference.begin()->first;
//         //     dbi::Record record = slottedPage->lookup(id);
//         //     ASSERT_EQ(std::string(record.data(), record.size()), reference.begin()->second);
//         //     std::string data = dbi::util::randomWord(random()%64);
//         //     if(slottedPage->tryInPageUpdate(id, dbi::Record(data))) {
//         //         reference.erase(reference.begin());
//         //         reference.insert(make_pair(id, data));
//         //     }
//         // }

//         // Do consistency check
//         if(true) {
//             auto records = slottedPage->getAllRecords(0); // page id does not matter
//             ASSERT_EQ(records.size(), reference.size());
//             for(auto& iter : records) {
//                 ASSERT_TRUE(reference.count(dbi::toRecordId(iter.first)) > 0);
//                 ASSERT_TRUE(iter.second == dbi::Record(reference.find(dbi::toRecordId(iter.first))->second));
//             }
//             continue;
//         }
//     }
//     free(slottedPage);
// }
