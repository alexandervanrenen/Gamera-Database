#ifndef __ExternalSort
#define __ExternalSort

#include "BufferManager.hpp"
#include "Run.hpp"
#include "OutputRun.hpp"
#include <string>
#include <list>
#include <stdint.h>
#include <memory>

namespace dbi {

class ExternalSort {
public:
	/// Setup sport engine
	ExternalSort(const std::string& inputFileName, const std::string& outputFileName, uint64_t pageSize, uint64_t maxMemory, bool showPerformance);

   /// Start the sort
   void run();
private:
	const std::string inputFileName;
	const std::string outputFileName;
	const uint64_t availablePages;
	const bool showPerformance;
   BufferManager buffer;

   /// Divide and conquer the algorithm !!!!
   std::list<std::unique_ptr<Run>> createRunsPhase();
   void mergeRuns(std::list<std::unique_ptr<Run>>& runs);
   void singleMergePhase(std::list<std::unique_ptr<Run>>& inputRuns, uint32_t numJoins, OutputRun& targetRun);
};

}

#endif
