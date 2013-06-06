#include "gtest/gtest.h"
#include "test/TestConfig.hpp"
#include "indexes/HashMap.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "buffer_manager/BufferManager.hpp"
#include <bitset>

using namespace std;
using namespace dbi;

TEST(HashMap, Simple)
{
   const uint32_t pages = 100;
   assert(kSwapFilePages>=pages);

   BufferManager bufferManager(kSwapFileName, pages / 2);
   SegmentManager segmentManager(bufferManager, true);
   SegmentId id = segmentManager.createSegment(SegmentType::HM, 10);
   HashMapSegment& segment = segmentManager.getHashMapSegment(id);

   int inputKey = 2;
   string inputValue = "hello world";
   string outputValue;

   dbi::HashMap<int, string> testTable(segment);

   testTable.insert(inputKey, inputValue);

   //    outputValue = testTable.get(inputKey);
   //    ASSERT_EQ(inputValue, outputValue);


   //    testTable.insert("first", "hello");
   //    string res = testTable.get("first");
   //    cout << "result from table: " << res << endl;
   //bitset<N>((num>>1) ^ num).to_string();

   //int x = 3;
   //cout << bitset<2>(x) << endl;
}
