#include "ExternalSort.hpp"
#include "Page.hpp"
#include "BufferManager.hpp"
#include "Run.hpp"
#include "OutputRun.hpp"
#include "RunHeap.hpp"
#include "util/Utility.hpp"
#include "FileNameProvider.hpp"
#include <array>
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
   // Phase I: Create runs
   auto startRunPhase = chrono::high_resolution_clock::now();
   auto runs = createRunsPhase();
   uint64_t initialRunCount = runs.size();
   auto endRunPhase = chrono::high_resolution_clock::now();

   // Phase II: Merge runs
   auto startMergePhase = chrono::high_resolution_clock::now();
   mergeRuns(runs);
   auto endMergePhase = chrono::high_resolution_clock::now();

   // Phase A: Show Performance
   if(showPerformance) {
      cout << "Run count: " << initialRunCount << endl;
      cout << "Run phase: " << chrono::duration_cast<chrono::milliseconds>(endRunPhase-startRunPhase).count() << "ms"  << endl;
      cout << "Merge phase: " << chrono::duration_cast<chrono::milliseconds>(endMergePhase-startMergePhase).count() << "ms" << endl;
      cout << "Both phases: " << chrono::duration_cast<chrono::milliseconds>(endMergePhase-startRunPhase).count() << "ms" << endl;
   }
}

list<unique_ptr<Run<uint64_t>>> ExternalSort::createRunsPhase()
{
   // Phase I: Create runs
   list<unique_ptr<Run<uint64_t>>> runs;
   uint64_t ioTime = 0;
   string runFileName = outputFileName + "yin";
   fstream inputFile(inputFileName, ios::binary | ios::in);
   fstream outputFile(runFileName, ios::binary | ios::out);
   for (uint64_t runId=0; true; runId++) {
      // Read data
      int64_t position = inputFile.tellg();
      auto start = chrono::high_resolution_clock::now();
      inputFile.read(buffer.begin(), buffer.size());
      inputFile.peek(); // Detect end of file
      bool readSuccessfull = inputFile.eof();
      auto end = chrono::high_resolution_clock::now();
      ioTime += chrono::duration_cast<chrono::nanoseconds>(end-start).count();
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
      auto run = dbiu::make_unique<Run<uint64_t>>(outputFile.tellg(), readBytes, runFileName);
      runs.push_back(move(run));
      outputFile.write(buffer.begin(), readBytes);
   }
      return runs;
}

void ExternalSort::singleMergePhase(list<unique_ptr<Run<uint64_t>>>& inputRuns, uint32_t numJoins, OutputRun<uint64_t>& targetRun)
{
   RunHeap<uint64_t> runHeap;
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

void ExternalSort::mergeRuns(list<unique_ptr<Run<uint64_t>>>& runs)
{
   FileNameProvider runName(outputFileName);
   while(!runs.empty()) {
      // Find nice merge strategy
      uint32_t minimalNumberOfMerges = ceil(runs.size() / (availablePages-1));
      uint32_t unusedSlots = (availablePages-1) - (runs.size() % (availablePages-1));
      if(minimalNumberOfMerges <= unusedSlots) {
         // Postpone as much work as possible
         unusedSlots -= minimalNumberOfMerges;
         string fileName = runName.getNext();
         OutputRun<uint64_t> targetRun1(fileName, true);
         singleMergePhase(runs, (availablePages-1)-unusedSlots, targetRun1);
         runs.push_back(targetRun1.createRun());

         // We can finish in this merge
         list<unique_ptr<Run<uint64_t>>> nextLevelRuns;
         while(runs.size() >= (availablePages-1)) {
            OutputRun<uint64_t> targetRun(fileName, true);
            singleMergePhase(runs, (availablePages-1), targetRun);
            runs.push_back(targetRun.createRun());
         }

         // Final merge pass
         OutputRun<uint64_t> targetRun2(outputFileName, false);
         singleMergePhase(runs, (availablePages-1), targetRun2);
         runs.clear();
      } else {
         // Just create the next level
         list<unique_ptr<Run<uint64_t>>> nextLevelRuns;
         string fileName = runName.getNext();
         while(!runs.empty()) {
            OutputRun<uint64_t> targetRun(fileName, true);
            singleMergePhase(runs, (availablePages-1), targetRun);
            nextLevelRuns.push_back(targetRun.createRun());
         }
         runs = move(nextLevelRuns);
      }
   }

   // Clean up temp files
   runName.removeAll();
}

}
