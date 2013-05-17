#include "SpinLock.hpp"

using namespace std;

namespace dbi {

namespace util {

SpinLock::SpinLock()
{
   if(pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE) == -1)
      throw;
}

SpinLock::~SpinLock()
{
   pthread_spin_destroy(&spinlock);
}

void SpinLock::lock()
{
   pthread_spin_lock(&spinlock);
}

void SpinLock::unlock()
{
   pthread_spin_unlock(&spinlock);
}

}

}
