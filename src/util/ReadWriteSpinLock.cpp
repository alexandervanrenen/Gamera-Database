#include "ReadWriteSpinLock.hpp"
#include <cassert>
#include <unistd.h>
#include <thread>

using namespace std;

namespace dbi {

namespace util {

ReadWriteSpinLock::ReadWriteSpinLock()
: readCount(0)
, writeCount(0)
{
}

ReadWriteSpinLock::~ReadWriteSpinLock()
{
   assert("clean up failed" && readCount == 0 && writeCount == 0);
}

void ReadWriteSpinLock::lockForReading()
{
   guard.lock();
   assert("read and write lock" && !(readCount != 0 && writeCount != 0));

   while(writeCount != 0)
   {
      guard.unlock();
      this_thread::yield();
      guard.lock();
   }

   readCount++;
   guard.unlock();
}

void ReadWriteSpinLock::lockForWriting()
{
   guard.lock();
   assert("read and write lock" && !(readCount != 0 && writeCount != 0));

   while(writeCount != 0 || readCount != 0)
   {
      guard.unlock();
      this_thread::yield();
      guard.lock();
   }
   writeCount++;
   guard.unlock();
}

bool ReadWriteSpinLock::tryLockForReading()
{
   guard.lock();
   assert("read and write lock" && !(readCount != 0 && writeCount != 0));

   if(writeCount == 0) {
      readCount++;
      guard.unlock();
      return true;
   } else {
      guard.unlock();
      return false;
   }
}

bool ReadWriteSpinLock::tryLockForWriting()
{
   guard.lock();
   assert("read and write lock" && !(readCount != 0 && writeCount != 0));

   if(writeCount == 0 && readCount == 0) {
      writeCount++;
      guard.unlock();
      return true;
   } else {
      guard.unlock();
      return false;
   }
}

void ReadWriteSpinLock::downgrade()
{
   guard.lock();
   assert("read and write lock" && !(readCount != 0 && writeCount != 0));
   assert(writeCount == 1);

   writeCount--;
   readCount++;
   guard.unlock();
}

void ReadWriteSpinLock::unlock()
{
   guard.lock();
   assert("no lock present" && (readCount != 0 || writeCount == 1));
   assert("read and write lock" && !(readCount != 0 && writeCount != 0));

   if(writeCount != 0)
      writeCount--;
   else
      readCount--;

   guard.unlock();
}

}

}
