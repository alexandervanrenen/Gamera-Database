#include "ExternalSort.hpp"
#include "util/Utility.hpp"
#include <set>
#include <iostream>
#include <utility>
#include <memory>

using namespace std;

void runComplexSort(uint64_t entriesCount, uint64_t pageSize, uint64_t maxMemory, bool showPerformance = false)
{
   if(!dbiu::createTestFile("bin/datain", entriesCount, [&](uint64_t i) {return rand();}))
   	throw;
   dbi::ExternalSort sorty;
   sorty.complexSort("bin/datain", "bin/dataout", pageSize, maxMemory, showPerformance);
}

int main()
{
	const uint32_t entriesCount = 1<<20;
	cout << "Running performance test .. (may take a while)" << endl;
	cout << "Sorting " << entriesCount << " entries (total " << (entriesCount*8) / 1024 << " KB)" << endl;
	cout << "Each page contains 256 Byte" << endl;
	cout << "The available number of pages is increased in each run" << endl;
	for(uint32_t i=2; i<9; i++) {
		cout << " -- " << endl;
		cout << "Test " << i - 1 << " (of " << 7 << "): " << (1<<i) << " pages (total " << (256 * (1<<i)) / 1000 << "KB)" << endl;
		runComplexSort(entriesCount, 256, 256 * (1<<i), true);
	}
}
