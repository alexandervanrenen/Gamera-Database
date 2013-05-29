#include "gtest/gtest.h"
#include "ext_hash_table/HashTable.hpp"
#include <bitset>

using namespace std;

//static const uint32_t kTestScale = 1;

TEST(HashTable, Everything)
{
    string inputKey = "first";
    string inputValue = "hello world";
    string outputValue;
    dbi::HashTable<std::string, std::string, std::hash<std::string>> testTable;
    
    testTable.insert(inputKey, inputValue);
    
//    outputValue = testTable.get(inputKey);
//    ASSERT_EQ(inputValue, outputValue);
    
    
//    testTable.insert("first", "hello");
//    std::string res = testTable.get("first");
//    cout << "result from table: " << res << endl;
    //bitset<N>((num>>1) ^ num).to_string();
    
    //int x = 3;
    //cout << bitset<2>(x) << endl;
}
