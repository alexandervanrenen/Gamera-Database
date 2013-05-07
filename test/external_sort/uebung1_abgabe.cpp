#include "external_sort/ExternalSort.hpp"
#include "external_sort/ExternalSortInterface.hpp"
#include "util/Math.hpp"
#include <iostream>
#include <string>
#include <cstdio>

using namespace std;

int main(int argc, char** argv) {
   // Check
   if(argc != 4) {
      cout << "wrong number of arguments" << endl;
      cout << "usage: ./sort <inputFile> <outputFile> <memoryBufferInByte>" << endl;
      return -1;
   }

   // Parse
   string input = argv[1];
   string output = argv[2];
   uint64_t memory = atoi(argv[3]);

   // Try to correct input (to small values or not multiple of page size)
   // The sort algorithm is not trained to handle stupid values :p
   memory -= memory % dbi::kPageSize;
   uint64_t pageSize = 4096;
   while(pageSize!=0 && (memory%pageSize != 0 || pageSize*3 > memory)) 
      pageSize /= 2;
   if(pageSize == 0) {
      cout << "sorry using to little memory .. " << endl;
      return -1;
   }

   // Run sort algorithm
   dbi::ExternalSort sorty(input, output, pageSize, memory, false);
   sorty.run();
   return 0;
}
