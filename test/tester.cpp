#include "gtest/gtest.h"
#include "TestConfig.hpp"
#include "util/Utility.hpp"
#include "common/Config.hpp"

using namespace std;

int main(int argc, char** argv)
{
   uint64_t fileLength = dbi::util::getFileLength(kSwapFileName);
   if(fileLength != kSwapFilePages * dbi::kPageSize) {
      cout << "creating swap file .. ";
      cout.flush();
      if(!dbi::util::createFile(kSwapFileName, kSwapFilePages * dbi::kPageSize)) {
         cout << "failed" << endl;
         throw;
      }
      cout << "done =)" << endl;
   } else {
      cout << "using existing swap file" << endl;
   }

   testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
