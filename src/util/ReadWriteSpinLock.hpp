#pragma once

#include "SpinLock.hpp"
#include <cstdint>

namespace dbi {

namespace util {

class ReadWriteSpinLock {
public:
   ReadWriteSpinLock();
   ~ReadWriteSpinLock();

   void lockForReading();
   void lockForWriting();

   bool tryLockForReading();
   bool tryLockForWriting();

   void downgrade();

   void unlock();

private:
   util::SpinLock guard;
   uint8_t readCount;
   uint8_t writeCount;

   ReadWriteSpinLock(const ReadWriteSpinLock&) = delete;
   ReadWriteSpinLock(const ReadWriteSpinLock&&) = delete;
   ReadWriteSpinLock& operator=(const ReadWriteSpinLock&) = delete;
};

}

}
