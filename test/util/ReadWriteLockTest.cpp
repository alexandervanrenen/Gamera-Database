#include "gtest/gtest.h"
#include "util/ReadWriteLock.hpp"
#include "test/TestConfig.hpp"
#include <thread>

TEST(ReadWriteLockTest, Simple)
{
   dbi::util::ReadWriteLock guard;
   guard.lockForReading();
   guard.lockForReading();
   ASSERT_FALSE(guard.tryLockForWriting());
   ASSERT_TRUE(guard.tryLockForReading());
   guard.unlock();
   guard.unlock();
   guard.unlock();
   guard.lockForWriting();
   ASSERT_FALSE(guard.tryLockForWriting());
   ASSERT_FALSE(guard.tryLockForReading());
   guard.unlock();
   ASSERT_TRUE(guard.tryLockForWriting());
   ASSERT_FALSE(guard.tryLockForReading());
   guard.downgrade();
   ASSERT_TRUE(guard.tryLockForReading());
   ASSERT_FALSE(guard.tryLockForWriting());
   guard.unlock();
   guard.unlock();
}

TEST(ReadWriteLockTest, Randomized)
{
   dbi::util::ReadWriteLock guard;
   std::thread t1([&guard]() {
      for(uint32_t i=0; i<1000; i++) {
         guard.lockForReading();
         usleep(10);
         guard.unlock();
      }
   });
   std::thread t2([&guard]() {
      for(uint32_t i=0; i<1000; i++) {
         guard.lockForWriting();
         usleep(10);
         guard.unlock();
      }
   });
   std::thread t3([&guard]() {
      for(uint32_t i=0; i<1000; i++) {
         guard.lockForWriting();
         guard.downgrade();
         usleep(10);
         guard.unlock();
      }
   });
   std::thread t4([&guard]() {
      for(uint32_t i=0; i<1000; i++) {
         if(guard.tryLockForReading()) {
            usleep(10);
            guard.unlock();
         }
      }
   });
   std::thread t5([&guard]() {
      for(uint32_t i=0; i<1000; i++) {
         if(guard.tryLockForWriting()) {
            usleep(10);
            guard.unlock();
         }
      }
   });

   t1.join();
   t2.join();
   t3.join();
   t4.join();
   t5.join();
}
