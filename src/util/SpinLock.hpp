#pragma once

#include <pthread.h>

namespace dbi {

namespace util {

class SpinLock {
public:
   SpinLock();
   ~SpinLock();

   void lock();
   void unlock();

private:
   pthread_spinlock_t spinlock;
};

}

}
