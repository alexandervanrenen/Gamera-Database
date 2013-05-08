#include "ExternalSort.hpp"
#include "Page.hpp"
#include "InputRun.hpp"
#include "OutputRun.hpp"
#include "RunHeap.hpp"
#include "util/Utility.hpp"
#include "util/StatisticsCollector.hpp"
#include "FileNameProvider.hpp"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cassert>
#include <list>
#include <memory>
#include <cstdio>
#include <chrono>
#include <set>
#include <queue>
#include <algorithm>

namespace dbi {

using namespace std;

ExternalSort::ExternalSort(const string& inputFileName, const string& outputFileName, uint64_t pageSize, uint64_t maxMemory, bool showPerformance)
: inputFileName(inputFileName)
, outputFileName(outputFileName)
, availablePages(maxMemory / pageSize)
, showPerformance(showPerformance)
, buffer(maxMemory, pageSize)
{
   // Check input
   assert(maxMemory % pageSize == 0);
   assert(maxMemory >= 3*pageSize);
}

void ExternalSort::run()
{
   StatisticsCollector<true> stats;

   // Phase I: Create runs
   stats.start("RunPhase");
   auto runs = createRunsPhase();
   uint64_t initialRunCount = runs.size();
   stats.end("RunPhase");

   // Phase II: Merge runs
   stats.start("Merge Phase");
   mergeRunPhase(runs);
   stats.end("Merge Phase");

   // Phase A: Show Performance
   if(showPerformance) {
      cout << "Run count: " << initialRunCount << endl;
      stats.print(cout);
   }
}

list<unique_ptr<InputRun>> ExternalSort::createRunsPhase()
{
   // Phase I: Create runs
   list<unique_ptr<InputRun>> runs;
   string runFileName = outputFileName + "yin";
   fstream inputFile(inputFileName, ios::binary | ios::in);
   if(!inputFile.is_open() || !inputFile.good())
      throw "unable to open input file";
   fstream outputFile(runFileName, ios::binary | ios::out);
   for (uint64_t runId=0; true; runId++) {
      // Read data
      int64_t position = inputFile.tellg();
      inputFile.read(buffer.begin(), buffer.size());
      inputFile.peek(); // Detect end of file
      bool readSuccessfull = inputFile.eof();
      inputFile.clear();
      int64_t readBytes = inputFile.tellg() - position;

      // End of file
      if(readSuccessfull) {
         // Trivial case -- buffer is larger than file
         if(runId == 0) {
            sort(reinterpret_cast<uint64_t*>(buffer.begin()), reinterpret_cast<uint64_t*>(buffer.begin()) + readBytes / sizeof(uint64_t));
            fstream resultFile(outputFileName, ios::binary | ios::out);
            resultFile.write(buffer.begin(), readBytes);
            return runs;
         }

         // Terminate
         if(readBytes <= 0)
            break;
      }

      // Sort and write
      sort(reinterpret_cast<uint64_t*>(buffer.begin()), reinterpret_cast<uint64_t*>(buffer.begin()) + readBytes / sizeof(uint64_t));
      auto run = dbiu::make_unique<InputRun>(outputFile.tellg(), readBytes, runFileName);
      runs.push_back(move(run));
      outputFile.write(buffer.begin(), readBytes);
   }
   return runs;
}

void ExternalSort::mergeSingleRun(list<unique_ptr<InputRun>>& inputRuns, uint32_t numJoins, OutputRun& targetRun)
{
   RunHeap runHeap;
   uint64_t totalBytes = 0;
   for (uint64_t i = 0; i < numJoins && !inputRuns.empty(); i++) {
      auto run = move(inputRuns.front());
      inputRuns.pop_front();
      run->assignPage(buffer.getPage(i));
      run->prepareForReading();
      totalBytes += run->size();
      runHeap.push(move(run));
   }

   // Set up output stream
   targetRun.assignPage(buffer.getPage(numJoins));
   targetRun.prepareForWriting();

   // Merge selected inputRuns
   while(runHeap.hasMore())
      targetRun.add(runHeap.getMin());

   // Add target run back to all inputRuns
   targetRun.flush();
}

void ExternalSort::mergeRunPhase(list<unique_ptr<InputRun>>& runs)
{
   FileNameProvider runName(outputFileName);
   while(!runs.empty()) {
      // Find nice merge strategy
      uint32_t minimalNumberOfMerges = ceil(runs.size() / (availablePages-1));
      uint32_t unusedPagesInLastRun = (availablePages-1) - (runs.size() % (availablePages-1));
      if(minimalNumberOfMerges <= unusedPagesInLastRun) { // => We can finish in this level
         string fileName = runName.getNext();

         // Postpone as much work as possible
         if(runs.size() >= (availablePages-1)) {
            uint32_t unusedPages = unusedPagesInLastRun - minimalNumberOfMerges;
            OutputRun targetRun1(fileName, true);
            mergeSingleRun(runs, (availablePages-1)-unusedPages, targetRun1);
            runs.push_back(targetRun1.convertToInputRun());
         }

         // Run full merge passes
         list<unique_ptr<InputRun>> nextLevelRuns;
         while(runs.size() >= (availablePages-1)) {
            OutputRun targetRun(fileName, true);
            mergeSingleRun(runs, (availablePages-1), targetRun);
            runs.push_back(targetRun.convertToInputRun());
         }

         // Final merge pass
         OutputRun targetRun2(outputFileName, false);
         mergeSingleRun(runs, (availablePages-1), targetRun2);
         runs.clear();
      } else { // => We can not finish in this level
         // Just create the next level
         list<unique_ptr<InputRun>> nextLevelRuns;
         string fileName = runName.getNext();
         while(!runs.empty()) {
            OutputRun targetRun(fileName, true);
            mergeSingleRun(runs, (availablePages-1), targetRun);
            nextLevelRuns.push_back(targetRun.convertToInputRun());
         }
         runs = move(nextLevelRuns);
      }
   }

   // Clean up temp files
   runName.removeAll();
}

}
