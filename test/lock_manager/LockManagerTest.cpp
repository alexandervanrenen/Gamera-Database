#include "test/TestConfig.hpp"
#include "util/Utility.hpp"
#include "common/Config.hpp"
#include <gtest/gtest.h>
#include "lock_manager/LockManager.hpp"
#include <thread>
#include <random>

void dowork(dbi::LockManager* lm, int thread) {
    for (uint64_t i=1; i < 100; i++) {
        lm->lock(dbi::TupleId(i), thread);
    }
    lm->unlockAll(thread);
}

TEST(LockManagerTest, ThreadTest) {
    dbi::LockManager lm;
    std::thread t1(dowork, &lm, 1);
    std::thread t2(dowork, &lm, 2);
    std::thread t3(dowork, &lm, 3);
    std::thread t4(dowork, &lm, 4);
    std::thread t5(dowork, &lm, 5);
    std::thread t6(dowork, &lm, 6);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    ASSERT_TRUE(lm.noLocks());
}


void randomTest(dbi::LockManager* lm, int thread) {
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(1,1000);
    auto rnd = std::bind(distribution, generator);
    for (uint64_t i=0; i < 10000; i++) {
        lm->lock(dbi::TupleId(rnd()), thread);
        if (i%10 == 0)
            lm->unlockAll(thread);
    }
}

TEST(LockManagerTest, RandomTest) {
    dbi::LockManager lm;
    std::thread t1(randomTest, &lm, 1);
    std::thread t2(randomTest, &lm, 2);
    std::thread t3(randomTest, &lm, 3);
    std::thread t4(randomTest, &lm, 4);
    std::thread t5(randomTest, &lm, 5);
    std::thread t6(randomTest, &lm, 6);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
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

