#include <gtest/gtest.h>
#include "ExternalSortTest.cpp"
#include "UtilityTest.cpp"

using namespace std;

int main(int argc, char** argv)
{
   testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
