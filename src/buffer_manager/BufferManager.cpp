#include "BufferManager.hpp"
#include "common/Config.hpp"
#include "util/Utility.hpp"
#include <fstream>
#include <cassert>
#include <bits/unordered_map.h>
#include <bits/hashtable.h>

namespace dbi {

using namespace std;

BufferManager::BufferManager(const std::string& fileName, uint64_t memoryPages)
: memoryPages(memoryPages)
, file(fileName.c_str(), ios::out | ios::in)
, allFrames(memoryPages)
, bufferFrameDir(memoryPages)
{
    // Check length of the file
    assert(file.is_open() && file.good());
    file.seekg(0, ios::end);
    size_t fileLength = file.tellg();
    file.seekg(0, ios::beg);
    assert(fileLength > 0 && fileLength%kPageSize==0);
    discPages = fileLength/kPageSize;

    // Allocate management data structures
    for(auto& iter : allFrames) {
        iter = dbiu::make_unique<BufferFrame>();
        freeFrames.push_back(iter.get());
    }
}

BufferFrame& BufferManager::fixPage(PageId pageId, bool exclusive)
{
    unique_lock<mutex> l(guard);

    // Case 1: Page is not loaded
    auto iter = loadedFrames.find(pageId);
    if(iter == loadedFrames.end()) {
        /// Wait if no page is free
        cond.wait(l, [&](){ return !freeFrames.empty() || !unusedFrames.empty(); });

        /// Swap out
        if(freeFrames.empty()) {
            BufferFrame* framePtr = unusedFrames.begin()->second;
            saveFrame(*framePtr);
            loadedFrames.erase(pageId);
            unusedFrames.erase(pageId);
            freeFrames.push_back(framePtr);
        }
        assert(!freeFrames.empty());

        // Get available frame
        BufferFrame& frame = *freeFrames.back();
        freeFrames.pop_back();
        loadFrame(pageId, frame);
        assert(frame.refCount == 0);
        frame.refCount++;
        frame.exclusive = exclusive;
        return frame;
    }

    // Case 2: Page is loaded and in any way reusable
    if((!iter->second->exclusive && !exclusive) || iter->second->refCount==0) {
        iter->second->exclusive = exclusive;
        if(iter->second->refCount==0)
            unusedFrames.erase(pageId);
        iter->second->refCount++;
        return *iter->second;
    }

    // Case 3: Otherwise
    iter->second->threadsWaiting++;
    while(iter->second->refCount != 0)
        iter->second->cond.wait(l);
    iter->second->refCount++;
    iter->second->exclusive = exclusive;
    iter->second->threadsWaiting--;
    return *iter->second;
}

BufferFrame& BufferManager::fixPage2(PageId pageId, bool exclusive){
    //check if page is already in memory
    auto bufferFrame = bufferFrameDir.find(pageId);  
    if(bufferFrame != nullptr){
        //page found
        return tryLockBufferFrame(*bufferFrame, pageId, exclusive);
    } else {
        //page not found -> load page from disc 
        pageLoadGuard.lock();
        //ensure that the page has not been loaded by another thread while we waited
        bufferFrame = bufferFrameDir.find(pageId);        
        if(bufferFrame != nullptr){
            //page has been loaded
            pageLoadGuard.unlock();
            return tryLockBufferFrame(*bufferFrame, pageId, exclusive);            
        } else {
            //page has not been loaded
            BufferFrame* bufferFrameToBeUsed;
            bool success = false;
            //find a buffer frame which can be used (either use unsed one or rule out another) and try to lock it
            //search another candidate whenever locking fails
            while(!success){
                //TODO: determine buffer frame to be used (either use unsed one or rule out another) -> store in bufferFrameToBeUsed                
                success = bufferFrameToBeUsed->accessGuard.tryLockForWriting();
            }
            PageId oldPageId = bufferFrameToBeUsed->pageId;
            if(bufferFrameToBeUsed->isDirty){
                saveFrame(*bufferFrameToBeUsed);
            }
            loadFrame(pageId, *bufferFrameToBeUsed);
            bufferFrameDir.updateKey(oldPageId, pageId);
            if(!exclusive){
                //if the lock is not request exclusively downgrade to read lock
                bufferFrameToBeUsed->accessGuard.downgrade();
            }
            pageLoadGuard.unlock();
            return *bufferFrameToBeUsed;
        }
    }
}

BufferFrame& BufferManager::tryLockBufferFrame(BufferFrame& bufferFrame, const PageId expectedPageId, const bool exclusive){
    //acquire rw-lock on provided buffer frame with respect to arg: exclusive
    if(exclusive){
        bufferFrame.accessGuard.lockForWriting();
    } else {
        bufferFrame.accessGuard.lockForReading();
    }
    //ensure that page is still within the locked buffer frame (another thread could have ruled it out while this one was waiting)
    if(bufferFrame.pageId == expectedPageId){
        return bufferFrame;
    } else {
        bufferFrame.accessGuard.unlock();
        return fixPage2(expectedPageId, exclusive);
    }
}

void BufferManager::unfixPage(BufferFrame& frame, bool isDirty)
{
    unique_lock<mutex> l(guard);
    assert(frame.refCount > 0);

    frame.isDirty |= isDirty;
    frame.refCount--;
    if(frame.refCount == 0 && frame.threadsWaiting == 0) {
        unusedFrames[frame.pageId] = &frame;
        cond.notify_one();
    } else {
        frame.cond.notify_one();
    }
}

void BufferManager::unfixPage2(BufferFrame& frame, bool isDirty){
    if(isDirty){
        saveFrame(frame);
    }    
    //TODO: update LRU queue stuff or second chance bits
    frame.accessGuard.unlock();
    
}

void BufferManager::flush()
{
    unique_lock<mutex> l(guard);
    for(auto iter : loadedFrames)
        if(iter.second->isDirty)
            saveFrame(*iter.second);
}

BufferManager::~BufferManager()
{
    flush();
}

void BufferManager::loadFrame(PageId pageId, BufferFrame& frame)
{
    assert(frame.refCount==0 && !frame.isDirty);
    file.seekg(pageId*kPageSize, ios::beg);
    file.read(frame.data.data(), kPageSize);
    frame.pageId = pageId;
    loadedFrames[pageId] = &frame;
}

void BufferManager::saveFrame(BufferFrame& frame)
{
    assert(frame.refCount==0);
    if(frame.isDirty) {
        file.seekg(frame.pageId*kPageSize, ios::beg);
        file.write(frame.data.data(), kPageSize);
    }
    frame.isDirty = false;
}

}
