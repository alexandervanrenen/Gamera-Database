#include "BufferManager.hpp"
#include "common/Config.hpp"
#include "util/Utility.hpp"
#include "util/StatisticsCollector.hpp"
#include "SwapOutSecondChance.hpp"
#include "SwapOutRandom.hpp"
#include "SwapOutTwoQueue.hpp"
#include <fstream>
#include <cassert>

namespace dbi {

using namespace std;

BufferManager::BufferManager(const std::string& fileName, uint64_t memoryPagesCount)
: memoryPagesCount(memoryPagesCount)
, file(fileName.c_str(), ios::out | ios::in)
, loadGuards(memoryPagesCount)
, bufferFrameDir(memoryPagesCount)
, swapOutAlgorithm(util::make_unique<SwapOutAlgorithm>())
, stats(util::make_unique<util::StatisticsCollector<collectPerformance>>("buffer manager"))
{
    // Check length of the file
    assert(file.is_open() && file.good());
    file.seekg(0, ios::end);
    size_t fileLength = file.tellg();
    file.seekg(0, ios::beg);
    assert(fileLength > 0 && fileLength%kPageSize==0);
    discPagesCount = fileLength/kPageSize;

    // Insert in map
    for(uint32_t i=0; i<memoryPagesCount; i++)
        bufferFrameDir.insert(10000000+i).pageId = 10000000+i;

    // Add to swap out strategy
    swapOutAlgorithm->initialize(bufferFrameDir);
}

BufferFrame& BufferManager::fixPage(PageId pageId, bool exclusive)
{
    // Check if page is already in memory
    BufferFrame* bufferFrame = bufferFrameDir.fuzzyFind(pageId);
    if(bufferFrame != nullptr)
        return tryLockBufferFrame(*bufferFrame, pageId, exclusive);

    // Otherwise: Load page from disc -- this may only be done by one thread
    loadGuards[pageId%memoryPagesCount].lock();

    // Ensure that the page has not been loaded by another thread while we waited
    bufferFrame = bufferFrameDir.find(pageId);
    if(bufferFrame != nullptr) {
        loadGuards[pageId%memoryPagesCount].unlock();
        return tryLockBufferFrame(*bufferFrame, pageId, exclusive);
    }

    // Find unused buffer frame (unused == not locked)
    bufferFrame = &swapOutAlgorithm->findPageToSwapOut(bufferFrameDir);

    // Replace page (write old and load new)
    PageId oldPageId = bufferFrame->pageId;
    fileGuard.lock();
    if(bufferFrame->isDirty)
        saveFrame(*bufferFrame);
    loadFrame(pageId, *bufferFrame);
    fileGuard.unlock();

    // Update map
    bufferFrameDir.updateKey(oldPageId, pageId);
    bufferFrame->pageId = pageId;
    if(!exclusive)
        bufferFrame->accessGuard.downgrade();
    loadGuards[pageId%memoryPagesCount].unlock();
    return *bufferFrame;
}

BufferFrame& BufferManager::tryLockBufferFrame(BufferFrame& bufferFrame, const PageId expectedPageId, const bool exclusive)
{
    // Acquire RW-lock on provided buffer frame with respect to argument: exclusive
    if(exclusive)
        bufferFrame.accessGuard.lockForWriting(); else
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
    //TODO: update LRU queue stuff or second chance bits
    swapOutAlgorithm->onUnfixPage(frame);
    frame.isDirty |= isDirty;
    frame.accessGuard.unlock();
}

void BufferManager::flush()
{
    // TODO: need locking ?
    for(auto& iter : bufferFrameDir.data())
        if(iter.value.isDirty)
            saveFrame(iter.value);
}

BufferManager::~BufferManager()
{
    flush();
    stats->print(cout);
}

void BufferManager::loadFrame(PageId pageId, BufferFrame& frame)
{
    assert(!frame.isDirty);
    stats->count("loads", 1);
    // file.seekg(pageId*kPageSize, ios::beg);
    // file.read(frame.data.data(), kPageSize);
    assert(file.good());
}

void BufferManager::saveFrame(BufferFrame& frame)
{
    if(frame.isDirty) {
        // file.seekg(frame.pageId*kPageSize, ios::beg);
        // file.write(frame.data.data(), kPageSize);
    }
    frame.isDirty = false;
}

}
