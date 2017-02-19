#pragma once

#include <pthread.h>

#ifdef __APPLE__
#include <libkern/OSAtomic.h>
#endif

namespace dbi {

namespace util {

class SpinLock {
public:
   SpinLock();
   ~SpinLock();

   void lock();
   void unlock();

private:
	#ifdef __APPLE__
   	OSSpinLock spinlock;
   #else
		pthread_spinlock_t spinlock;
   #endif
};

}

}
