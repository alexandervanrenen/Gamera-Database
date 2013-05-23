#include <stdint.h>
#include "gtest/gtest.h"
#include "segment_manager/SlottedPage.hpp"
#include "common/Config.hpp"
#include "util/Utility.hpp"
#include <cstdlib>
#include <unordered_map>
#include <algorithm>

TEST(SlottedPage, SlotReuseAfterDelete){
    dbi::SlottedPage* slottedPage = static_cast<dbi::SlottedPage*>(malloc(dbi::kPageSize));
    slottedPage->initialize();
    
    std::string data = "Hello World!";
    dbi::Record dataRecord(data);
    
    dbi::RecordId dataRecordId1 = slottedPage->insert(dataRecord);
    ASSERT_TRUE(slottedPage->remove(dataRecordId1));
    dbi::RecordId dataRecordId2 = slottedPage->insert(dataRecord);
    ASSERT_EQ(dataRecordId1, dataRecordId2);
    
    free(slottedPage);
}

TEST(SlottedPage, DefragmentationBasic){
    
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
    while(slottedPage->getBytesFreeForRecord() >= staticData.length()){
        slottedPage->insert(dbi::Record(staticData));
    }
    
    uint16_t freeSpaceBeforeNewDataInsert = slottedPage->getBytesFreeForRecord();
    ASSERT_TRUE(slottedPage->getBytesFreeForRecord() < newData.length());  
    ASSERT_TRUE(slottedPage->remove(fragmentationRecordId1));
    ASSERT_TRUE(slottedPage->remove(fragmentationRecordId2));
    ASSERT_TRUE(slottedPage->getBytesFreeForRecord() >= newData.length());
    
    // The page should now have a structure like: <freeSpace> <staticData> <freeSpace> (whereby both free space areas are to short for the new record)    
    slottedPage->defragment();
    
    // No side effects on sample record
    ASSERT_EQ(slottedPage->lookup(staticRecordId).data(), staticData);
    //ASSERT_EQ(slottedPage->getBytesFreeForRecord(), newData.length() + freeSpaceBeforeNewDataInsert);
    
    
    ASSERT_TRUE(slottedPage->getBytesFreeForRecord() >= newData.length());
    dbi::RecordId newDataRecordId = slottedPage->insert(newDataRecord);
    ASSERT_EQ(slottedPage->lookup(newDataRecordId).data(), newData);
    // New record is as long as the two removed ones
    ASSERT_EQ(freeSpaceBeforeNewDataInsert, slottedPage->getBytesFreeForRecord());
    
    free(slottedPage);
}

TEST(SlottedPage, Randomized)
{
    std::unordered_multimap<dbi::RecordId, std::string> reference;
    dbi::SlottedPage* slottedPage = static_cast<dbi::SlottedPage*>(malloc(dbi::kPageSize));
    slottedPage->initialize();

    for(uint32_t i=0; i<10000; i++) {
        uint32_t operation = random() % 100;

        // Do insert
        if(operation <= 40) {
            std::string data = dbi::util::randomWord(random()%64);
            if(slottedPage->getBytesFreeForRecord() < data.size())
                continue;
            dbi::RecordId id = slottedPage->insert(dbi::Record(data));
            reference.insert(make_pair(id, data));
            continue;
        }

        // Do remove
        if(operation <= 70) {
            if(reference.empty())
                continue;
            dbi::RecordId id = reference.begin()->first;
            dbi::Record record = slottedPage->lookup(id);
            ASSERT_EQ(std::string(record.data(), record.size()), reference.begin()->second);
            slottedPage->remove(id);
            reference.erase(reference.begin());
        }

        // Do update
        if(operation <= 98) {
            if(reference.empty())
                continue;
            dbi::RecordId id = reference.begin()->first;
            dbi::Record record = slottedPage->lookup(id);
            ASSERT_EQ(std::string(record.data(), record.size()), reference.begin()->second);
            std::string data = dbi::util::randomWord(random()%64);
            if(slottedPage->tryInPageUpdate(id, dbi::Record(data))) {
                reference.erase(reference.begin());
                reference.insert(make_pair(id, data));
            }
        }

        // Do consistency check
        if(operation <= 99) {
            std::vector<dbi::Record> records = slottedPage->getAllRecords();
            ASSERT_EQ(records.size(), reference.size());
            for(auto& iter : records)
                std::any_of(reference.begin(), reference.end(), [&iter]
                    (const std::pair<dbi::RecordId, std::string>& ref) {
                    return ref.second == std::string(iter.data(), iter.size());
                });
        }
    }
    free(slottedPage);
}
