#include "ExternalSort.hpp"
#include "Page.hpp"
#include "BufferManager.hpp"
#include "Run.hpp"
#include "util/Utility.hpp"
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

namespace dbi {

using namespace std;

/// Enable or disable performance reporting
static const bool showPerformance = true;

ExternalSort::ExternalSort()
{
}

template<class T>
void simpleSortImpl(const string& inputFileName, const string& outputFileName)
{
   // Open file
   ifstream in(inputFileName, ios::binary);
   if (!in.is_open() || !in.good())
      throw;

   // Figure out file length and entry count
   in.seekg(0, ios::end);
   uint64_t fileLength = static_cast<uint64_t>(in.tellg());
   in.seekg(0, ios::beg);
   if (fileLength % sizeof(T) != 0)
      throw;
   uint64_t entryCount = fileLength / sizeof(T);

   // Read into buffer
   vector<T> buffer(entryCount);
   in.read(reinterpret_cast<char*>(buffer.data()), fileLength);

   // Sort
   sort(buffer.begin(), buffer.end());

   // Open output and write data
   ofstream out(outputFileName, ios::binary);
   if (!out.is_open() || !out.good())
      throw;
   out.write(reinterpret_cast<char*>(buffer.data()), fileLength);
}

template<class T>
void complexSortImpl(const string& fileName, uint64_t maxMemory, uint64_t pageSize)
{
   // Check input and calculate constants
   assert(maxMemory % pageSize == 0);
   assert(maxMemory > 2*pageSize);
   const uint64_t availablePages = maxMemory / pageSize;

   // Set up buffers
   BufferManager<T> buffer(maxMemory, pageSize);
   list<unique_ptr<Run<T>>> runs;

   // Phase I: Create runs
   auto startRunPhase = chrono::high_resolution_clock::now();
   auto originalFile = make_shared<fstream>(fileName, ios::binary | ios::out | ios::in);
   for (uint64_t runId=0; true; runId++) {
      // Read data
      int64_t position = originalFile->tellg();
      originalFile->read(buffer.begin(), maxMemory);
      int64_t readBytes = originalFile->tellg() - position;
      originalFile->clear();
      if (readBytes <= 0)
         break;

      // Sort and write
      sort(reinterpret_cast<uint64_t*>(buffer.begin()), reinterpret_cast<uint64_t*>(buffer.begin()) + readBytes / sizeof(T));
      auto runFileName = fileName + to_string(runId);
      fstream runFile(runFileName, ios::binary | ios::out);
      runFile.write(buffer.begin(), readBytes);
      auto run = dbiu::make_unique<Run<T>>(0, readBytes, runFileName);
      runs.push_back(move(run));
   }
   auto endRunPhase = chrono::high_resolution_clock::now();
   uint64_t initialRunCount = runs.size();

   // Phase II: Merge runs
   auto startMergePhase = chrono::high_resolution_clock::now();
   while (runs.size() > 1) {
      // Select runs for this merge phase
      vector<unique_ptr<Run<T>>> workRuns;
      uint64_t totalBytes = 0;
      for (uint64_t i = 0; i < availablePages - 1 && !runs.empty(); i++) {
         auto run = move(runs.front());
         runs.pop_front();
         run->assignPage(buffer.getPage(i));
         run->prepareForReading();
         totalBytes += run->size();
         workRuns.push_back(move(run));
      }

      // Set up output stream
      auto targetFileName = fileName + ((runs.size()==0) ? "" : "_merge_" + to_string(runs.size()));
      auto targetRun = dbiu::make_unique<Run<T>>(0, totalBytes, targetFileName);
      targetRun->assignPage(buffer.getPage(availablePages - 1));
      targetRun->prepareForWriting();

      // Merge selected runs
      while(!workRuns.empty()) {
         // Find run with min value -- using linear search (faster till at least 1024 entries)
         T minValue = workRuns[0]->peekNext();
         uint64_t minIndex = 0;
         for (uint64_t i = 1; i < workRuns.size(); i++) {
            if (workRuns[i]->peekNext() < minValue) {
               minValue = workRuns[i]->peekNext();
               minIndex = i;
            }
         }

         // Remove
         T value = workRuns[minIndex]->getNext();
         if (!workRuns[minIndex]->hasNext()) {
            remove(workRuns[minIndex]->getFileName().c_str());
            workRuns.erase(workRuns.begin() + minIndex);
         }

         // Add to result
         targetRun->add(value);
      }
      targetRun->flush();
      runs.push_back(move(targetRun));
   }
   auto endMergePhase = chrono::high_resolution_clock::now();

   if(showPerformance) {
      cout << "run count: " << initialRunCount << endl;
      cout << "run phase: " << chrono::duration_cast<chrono::milliseconds>(endRunPhase-startRunPhase).count() << " ms" << endl;
      cout << "merge phase: " << chrono::duration_cast<chrono::milliseconds>(endMergePhase-startMergePhase).count() << " ms" << endl;
   }
}

void ExternalSort::simpleSort(const string& inputFileName, const string& outputFileName)
{
   simpleSortImpl<uint64_t>(inputFileName, outputFileName);
}

void ExternalSort::complexSort(const string& fileName, uint64_t maxMemory, uint64_t pageSize)
{
   complexSortImpl<uint64_t>(fileName, maxMemory, pageSize);
}

}
