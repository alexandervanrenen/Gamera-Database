#include "test/TestConfig.hpp"
#include "util/Utility.hpp"
#include "common/Config.hpp"
#include <gtest/gtest.h>
#include "lock_manager/LockManager.hpp"
#include <thread>
#include <random>
#include <array>

void threadTest(dbi::LockManager* lm, int thread) {
    for (uint64_t i=1; i < 100; i++) {
        try {
            lm->lock(dbi::TupleId(i), thread);
        } catch (dbi::DeadlockException e) {}
    }
    lm->unlockAll(thread);
}

TEST(LockManagerTest, ThreadTest) {
    dbi::LockManager lm;
    const int numthreads = 20;
    std::array<std::thread, numthreads> threads;
    // Start threads
    for (int i=0; i < numthreads; i++)
        threads[i] = std::thread(threadTest, &lm, i);
    // Wait for threads to finish
    for (int i=0; i < numthreads; i++)
        threads[i].join();
    ASSERT_TRUE(lm.noLocks());
}


void randomTest(dbi::LockManager* lm, int thread) {
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1,100);
    auto rnd = std::bind(distribution, generator);
    for (uint64_t i=1; i <= 10000; i++) {
        try {
            lm->lock(dbi::TupleId(rnd()), thread);
        } catch (dbi::AlreadyLockedException e) {
        } catch (dbi::DeadlockException e) {
            lm->unlockAll(thread);
        }
        if (i%10 == 0)
            lm->unlockAll(thread);
    }
    lm->unlockAll(thread);

}

TEST(LockManagerTest, RandomTest) {
    dbi::LockManager lm;
    const int numthreads = 20;
    std::array<std::thread, numthreads> threads;
    // Start threads
    for (int i=0; i < numthreads; i++)
        threads[i] = std::thread(randomTest, &lm, i);
    // Wait for threads to finish
    for (int i=0; i < numthreads; i++)
        threads[i].join();
    ASSERT_TRUE(lm.noLocks());

}


TEST(LockManagerTest, SimpleTest) {
    typedef dbi::TupleId TupleId;
    dbi::LockManager m;
    ASSERT_TRUE(m.lock(TupleId(1), 1));
    ASSERT_FALSE(m.tryLock(TupleId(1), 2));
    ASSERT_TRUE(m.lock(TupleId(2), 1));
    ASSERT_FALSE(m.noLocks());
    m.unlockAll(1);
    ASSERT_TRUE(m.noLocks());
    ASSERT_TRUE(m.lock(TupleId(1), 2));
    ASSERT_FALSE(m.noLocks());
    m.unlockAll(2);
    ASSERT_TRUE(m.noLocks());
}

TEST(LockManagerTest, Upgrade) {
    typedef dbi::TupleId TupleId;
    dbi::LockManager m;
    ASSERT_TRUE(m.lock(TupleId(1), 1));
    ASSERT_FALSE(m.lock(TupleId(1), 1));
    // Shared lock on 2
    ASSERT_TRUE(m.lock(TupleId(2), 1, false));
    ASSERT_TRUE(m.lock(TupleId(2), 2, false));
    m.unlockAll(1);
    // Lock upgrade on 2
    ASSERT_TRUE(m.lock(TupleId(2), 2, true));
    m.unlockAll(2);
    ASSERT_TRUE(m.noLocks());
}

