#include "ReadWriteLock.hpp"
#include <cassert>

using namespace std;

namespace dbi {

namespace util {

ReadWriteLock::ReadWriteLock()
: readCount(0)
, writeCount(0)
{
}

ReadWriteLock::~ReadWriteLock()
{
   assert("clean up failed" && readCount == 0 && writeCount == 0);
}

void ReadWriteLock::lockForReading()
{
   unique_lock<mutex> l(guard);
   assert("read and write lock" && !(readCount != 0 && writeCount != 0));

   while(writeCount != 0)
      waiterQueue.wait(l);
   readCount++;
}

void ReadWriteLock::lockForWriting()
{
   unique_lock<mutex> l(guard);
   assert("read and write lock" && !(readCount != 0 && writeCount != 0));

   while(writeCount != 0 || readCount != 0)
      waiterQueue.wait(l);
   writeCount++;
}

bool ReadWriteLock::tryLockForReading()
{
   unique_lock<mutex> l(guard);
   assert("read and write lock" && !(readCount != 0 && writeCount != 0));

   if(writeCount == 0) {
      readCount++;
      return true;
   } else {
      return false;
   }
}

bool ReadWriteLock::tryLockForWriting()
{
   unique_lock<mutex> l(guard);
   assert("read and write lock" && !(readCount != 0 && writeCount != 0));

   if(writeCount == 0 && readCount == 0) {
      writeCount++;
      return true;
   } else {
      return false;
   }
}

void ReadWriteLock::downgrade()
{
   unique_lock<mutex> l(guard);
   assert("read and write lock" && !(readCount != 0 && writeCount != 0));
   assert(writeCount == 1);

   writeCount--;
   readCount++;
   waiterQueue.notify_all();
}

void ReadWriteLock::unlock()
{
   unique_lock<mutex> l(guard);
   assert("no lock present" && (readCount != 0 || writeCount == 1));
   assert("read and write lock" && !(readCount != 0 && writeCount != 0));

   if(writeCount != 0)
      writeCount--;
   else
      readCount--;

   waiterQueue.notify_all();
}

}

}
