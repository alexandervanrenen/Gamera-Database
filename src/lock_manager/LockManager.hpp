#pragma once

#include <signal.h>
#include <iostream>
#include <functional>
#include <list>
#include <condition_variable>
#include <mutex>
#include <algorithm>
#include "tbb/concurrent_hash_map.h"
#include "common/Config.hpp"
#include "common/TupleId.hpp"
#include "util/Graph.hpp"

namespace dbi {

typedef uint64_t TxId;

struct HashCompareTupleId {
    static size_t hash( const TupleId& x ) {
        return x.toInteger();
    }
    static bool equal( const TupleId& x, const TupleId& y ) {
        return x==y;
    }
};

struct HashCompareTxId {
    static size_t hash( const TxId& x ) {
        return x;
    }
    static bool equal( const TxId& x, const TxId& y ) {
        return x==y;
    }
};

class DeadlockException {
};

class AlreadyLockedException {
};


class LockManager {
public:
    bool lock(TupleId tid, TxId tx, bool exclusive=true);
    bool tryLock(TupleId tid, TxId tx, bool exclusive=true);
    void unlockAll(TxId tx);
    bool noLocks() {
        return locks.empty();
    };
private:
    typedef std::list<TxId> TxList;
    typedef std::list<TupleId> TidList;
    typedef std::pair<std::condition_variable*, std::mutex*> Waiter;
    typedef tbb::concurrent_hash_map<TupleId, bool, HashCompareTupleId> TupleBoolMap;
    typedef tbb::concurrent_hash_map<TupleId, TxList, HashCompareTupleId> TupleTxMap;
    typedef tbb::concurrent_hash_map<TupleId, Waiter, HashCompareTupleId> TupleWaiterMap;
    typedef tbb::concurrent_hash_map<TxId, TidList, HashCompareTxId> TxTupleMap;
    typedef tbb::concurrent_hash_map<TxId, TupleId, HashCompareTxId> TxTidMap;
    TupleBoolMap locks; // which tuples are locked
    TupleTxMap transactions; // which tuple is locked by which transactions
    TxTupleMap lockedtuples; // which transaction has which tuples locked
    TupleWaiterMap waiterlocks; // condition_variable + mutex for waiting on tuple
    TupleTxMap waiterlist; // Transactions which are waiting on a tuple
    Graph<TxId> waitgraph;

    void unlock(TxId tx, TupleId tid);
    bool deadlock(TxId tx, TupleId tid);
};

}
