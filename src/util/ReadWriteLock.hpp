#pragma once

#include <condition_variable>
#include <mutex>

namespace dbi {

namespace util {

class ReadWriteLock {
public:
   ReadWriteLock();
   ~ReadWriteLock();

   void lockForReading();
   void lockForWriting();

   bool tryLockForReading();
   bool tryLockForWriting();

   void downgrade();

   void unlock();

private:
   std::mutex guard;
   uint8_t readCount;
   uint8_t writeCount;
   std::condition_variable waiterQueue;

   ReadWriteLock(const ReadWriteLock&) = delete;
   ReadWriteLock(const ReadWriteLock&&) = delete;
   ReadWriteLock& operator=(const ReadWriteLock&) = delete;
};

}

}
