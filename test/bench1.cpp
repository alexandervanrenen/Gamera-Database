#include "util/StatisticsCollector.hpp"
#include "util/Utility.hpp"
#include "util/Math.hpp"
#include "util/WinnerTree.hpp"
#include "external_sort/ExternalSort.hpp"
#include <cstdio>
#include <iostream>
#include <algorithm>

using namespace std;

void baseline(const uint32_t entriesCount);
void bench();

int main() {
   // baseline(100*1024*1024/sizeof(uint64_t));
   StatisticsCollector stats;
   WinnerTree<uint32_t, 99> tree(4);
   tree.set(0, 3);
   tree.set(1, 1);
   tree.set(2, 6);
   tree.set(3, 2);
   cout << tree.min() << endl;
   
   throw;

   // vector<uint64_t> data(400 * 1024 * 1024 / 8);
   // for(auto& iter : data)
   //    iter = rand();

   // stats.start("asd");
   // sort(data.begin(), data.end());
   // stats.end("asd");

   // stats.print();
//   bench();
}

void bench() {
   const uint32_t entriesCount = 100 * 1024 * 1024 / sizeof(uint64_t);
   const uint32_t pageSize = 4 * 1024;
   const uint32_t maxMemory = 10 * 1024 * 1024;

   dbiu::createTestFile("bin/datain", entriesCount, [](uint64_t) {return rand();});
   StatisticsCollector stats;

   for(uint32_t i=0; i<1; i++) {
      stats.start("sort");
      dbi::ExternalSort sorty("bin/datain", "bin/dataout", pageSize, maxMemory, true);
      sorty.run();
      stats.end("sort");
   }

   stats.print();
   remove("bin/datain");
}

void baseline(const uint32_t entriesCount) {

   dbiu::createTestFile("bin/datain", entriesCount, [](uint64_t) {return rand();});
   vector<uint8_t> data(entriesCount * sizeof(uint64_t));
   for(auto& iter : data) iter = 0; // touch memory
   StatisticsCollector stats;

   for(uint32_t i=0; i<10; i++) {
      stats.start("read");
      ifstream in("bin/datain");
      in.read(reinterpret_cast<char*>(data.data()), entriesCount * sizeof(uint64_t));
      stats.end("read");
      if(system("/sbin/sysctl -w vm.drop_caches=3"))
         cout << "A2" << endl;
   }

   // for(uint32_t i=0; i<10; i++) {
   //    stats.start("write");
   //    ofstream out("bin/datain");
   //    out.write(reinterpret_cast<char*>(data.data()), entriesCount * sizeof(uint64_t));
   //    stats.end("write");
   // }

   stats.print();
   remove("bin/datain");
}
