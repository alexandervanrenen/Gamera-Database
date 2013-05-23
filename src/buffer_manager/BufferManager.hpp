#pragma once

#include "BufferFrame.hpp"
#include "common/Config.hpp"
#include "util/ConcurrentOffsetHash.hpp"
#include "util/SpinLock.hpp"
#include <memory>
#include <mutex>
#include <string>

namespace dbi {

namespace util {
template<bool>
class StatisticsCollector;
}
class SwapOutSecondChance;
class SwapOutRandom;
class SwapOutTwoQueue;

class BufferManager {
public:
   /// Constructor: The complete number of pages (on disc are given by the file size)
   BufferManager(const std::string& filename, uint64_t memoryPagesCount);

   /// A method to retrieve frames given a page ID and indicating whether the page will be held exclusively by this thread or not.
   BufferFrame& fixPage(PageId pageId, bool exclusive);

   /// Return a frame to the buffer manager indicating whether it is dirty or not.
   void unfixPage(BufferFrame& frame, bool isDirty);

   /// Access maximum number of pages
   uint64_t getNumMemoryPages()
   {
      return memoryPagesCount;
   }
   uint64_t getNumDiscPages()
   {
      return discPagesCount;
   }

   // Write all data to disc
   void flush();

   /// Destructor. Write all dirty frames to disk and free all resources.
   ~BufferManager();

private:
   /// Configuration
   using LockType = util::SpinLock; // Use std::mutex when profiling with valgrind
   using SwapOutAlgorithm = SwapOutSecondChance;

   // Constants
   uint64_t memoryPagesCount;
   uint64_t discPagesCount;
   int fileFD;

   /// Indicates that pageId%numPages is currently loading
   std::vector<LockType> loadGuards;

   /// Points from a page id to the buffer frame containing this page
   util::ConcurrentOffsetHash<PageId, BufferFrame> bufferFrameDir;

   /// Algorithm to find a page which can be swapped out
   std::unique_ptr<SwapOutAlgorithm> swapOutAlgorithm;

   /// Helper
   void loadFrame(PageId pageId, BufferFrame& frame);
   void saveFrame(BufferFrame& frame);

   /// Tries to lock a buffer frame in which the provided pageId is expected
   /// For the purpose of concurrency protection, the pageId of the buffer frame is compared with the provided one after the lock has been aquired.
   /// In case the pageId does not match, the lock is released and fixPage is called instead.
   /// Otherwise the locked buffer frame is returned.
   BufferFrame& tryLockBufferFrame(BufferFrame& bufferFrame, const PageId expectedPageId, const bool exclusive);

   /// Collecting performance
   const static bool collectPerformance = false;
   std::unique_ptr<util::StatisticsCollector<collectPerformance>> stats;
};

const static bool kExclusive = true;
const static bool kShared = false;
const static bool kClean = false;
const static bool kDirty = true;

}
