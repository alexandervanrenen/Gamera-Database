#include "gtest/gtest.h"
#include "UtilityTest.cpp"
#include "ExternalSortTest.cpp"
#include "BufferManagerTest.hpp"
#include "SegmentManagerTest.hpp"

using namespace std;

int main(int argc, char** argv)
{
   testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
