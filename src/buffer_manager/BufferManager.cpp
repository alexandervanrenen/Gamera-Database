#include "BufferManager.hpp"
#include "common/Config.hpp"
#include "SwapOutRandom.hpp"
#include "SwapOutSecondChance.hpp"
#include "SwapOutTwoQueue.hpp"
#include "util/StatisticsCollector.hpp"
#include "util/Utility.hpp"
#include <cassert>
#include <fcntl.h>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>

namespace dbi {

using namespace std;

BufferManager::BufferManager(const std::string& fileName, uint64_t memoryPagesCount)
: memoryPagesCount(memoryPagesCount)
, loadGuards(memoryPagesCount)
, bufferFrameDir(memoryPagesCount)
, swapOutAlgorithm(util::make_unique<SwapOutAlgorithm>())
, stats(util::make_unique<util::StatisticsCollector<collectPerformance>>("buffer manager"))
{
   // Check length of the file
   uint64_t length = util::getFileLength(fileName);
   if(length == 0 || length%kPageSize!=0) {
      assert(length > 0 && length%kPageSize==0);
      throw;
   }
   discPagesCount = length / kPageSize;

   // Open file
   fileFD = open(fileName.c_str(), O_RDWR);
   if(fcntl(fileFD, F_GETFL) == -1)
      assert("can not open file" && false);

   // Insert in map
   for(uint32_t i = 0; i < memoryPagesCount; i++)
      bufferFrameDir.insert(PageId(10000000 + i)).pageId = PageId(10000000 + i); // TODO: WTF !?

   // Add to swap out strategy
   swapOutAlgorithm->initialize(bufferFrameDir);
}

BufferFrame& BufferManager::fixPage(PageId pageId, bool exclusive)
{
   // Check if page is already in memory
   BufferFrame* bufferFrame = bufferFrameDir.fuzzyFind(pageId);
   if(bufferFrame != nullptr)
      return tryLockBufferFrame(*bufferFrame, pageId, exclusive);

   // Otherwise: Load page from disc -- ensure that this page is not loaded several times
   loadGuards[pageId.toInteger() % loadGuards.size()].lock();

   // Ensure that the page has not been loaded by another thread while we waited
   bufferFrame = bufferFrameDir.find(pageId);
   if(bufferFrame != nullptr) {
      loadGuards[pageId.toInteger() % loadGuards.size()].unlock();
      return tryLockBufferFrame(*bufferFrame, pageId, exclusive);
   }

   // Find unused buffer frame (unused == not locked) and lock it for writing
   bufferFrame = &swapOutAlgorithm->findPageToSwapOut(bufferFrameDir);

   // Replace page (write old and load new)
   PageId oldPageId = bufferFrame->pageId;
   if(bufferFrame->isDirty)
      saveFrame(*bufferFrame);
   loadFrame(pageId, *bufferFrame);

   // Update map
   bufferFrameDir.updateKey(oldPageId, pageId);
   bufferFrame->pageId = pageId;
   if(!exclusive)
      bufferFrame->accessGuard.downgrade();
   loadGuards[pageId.toInteger() % loadGuards.size()].unlock();
   return *bufferFrame;
}

BufferFrame& BufferManager::tryLockBufferFrame(BufferFrame& bufferFrame, const PageId expectedPageId, const bool exclusive)
{
   // Acquire RW-lock on provided buffer frame with respect to argument: exclusive
   if(exclusive)
      bufferFrame.accessGuard.lockForWriting();
   else
      bufferFrame.accessGuard.lockForReading();

   // Ensure that the loaded page has not changed (another thread could have ruled it out while this one was waiting)
   if(bufferFrame.pageId == expectedPageId) {
      swapOutAlgorithm->onFixPage(bufferFrame);
      return bufferFrame;
   } else {
      stats->count("bad frame read", 1);
      bufferFrame.accessGuard.unlock();
      return fixPage(expectedPageId, exclusive);
   }
}

void BufferManager::unfixPage(BufferFrame& frame, bool isDirty)
{
   swapOutAlgorithm->onUnfixPage(frame);
   frame.isDirty |= isDirty;
   frame.accessGuard.unlock();
}

void BufferManager::flush()
{
   for(auto& iter : bufferFrameDir.data())
      if(iter.value.isDirty)
         saveFrame(iter.value);
}

BufferManager::~BufferManager()
{
   flush();
   stats->print(cout);
   close(fileFD);
}

void BufferManager::loadFrame(PageId pageId, BufferFrame& frame)
{
   assert(!frame.isDirty);
   stats->count("loads", 1);
   if(pread(fileFD, frame.pageContent.data(), kPageSize, pageId.toInteger() * kPageSize) != kPageSize)
      throw;
}

void BufferManager::saveFrame(BufferFrame& frame)
{
   if(frame.isDirty)
      if(pwrite(fileFD, frame.pageContent.data(), kPageSize, frame.pageId.toInteger() * kPageSize) != kPageSize)
         throw;
   frame.isDirty = false;
}

}
