
#include <cassert>
#include "LockManager.hpp"

namespace dbi {

bool LockManager::tryLock(TupleId tid, TxId tx, bool exclusive) {
    TupleBoolMap::accessor a;
    bool res = locks.insert(a, {tid, exclusive});
    if (res) {
        TupleTxMap::accessor atx;
        bool restx = transactions.insert(atx, tid);
        atx->second.push_back(tx);
        TxTupleMap::accessor atup;
        lockedtuples.insert(atup, tx);
        atup->second.push_back(tid);
        return true;
    } else if (!exclusive && !a->second) {
        TupleTxMap::accessor atx;
        bool restx = transactions.insert(atx, tid);
        atx->second.push_back(tx);
        TxTupleMap::accessor atup;
        lockedtuples.insert(atup, tx);
        atup->second.push_back(tid);
        return true;
    } else {
        TupleTxMap::const_accessor atx;
        if (transactions.find(atx, tid)) {
            if (std::find(atx->second.cbegin(), atx->second.cend(), tx) != atx->second.cend())
                throw new AlreadyLockedException();
        }
    }
    return false;
}

bool LockManager::lock(TupleId tid, TxId tx, bool exclusive) {
    bool first = true;
    while (true) {
        if (tryLock(tid, tx, exclusive))
            break;
        printf("T%d waiting for %d\n",  int(tx), int(tid.toInteger()));
        // Tuple is locked, wait
        if (deadlock(tx, tid))
            throw new DeadlockException(); 
        if (first) {
            TupleTxMap::accessor atup;
            waiterlist.insert(atup, tid);
            atup->second.push_back(tx);
            TxTidMap::accessor atx;
            waitingfor.insert(atx, {tx, tid});
            first = false;
        }
        TupleWaiterMap::accessor a;
        if (waiterlocks.insert(a, tid)) {
            a->second.first = new std::condition_variable;
            a->second.second = new std::mutex;
        }
        std::unique_lock<std::mutex> ul(*a->second.second);
        std::condition_variable* cv = a->second.first;
        a.release();
        cv->wait(ul);
    }
    if (!first) {
        TupleTxMap::accessor atup;
        waiterlist.find(atup, tid);
        atup->second.remove(tx);
        TxTidMap::accessor atx;
        waitingfor.find(atx, tx);
        waitingfor.erase(atx);
    }
    printf("T%d locked %d\n",  int(tx), int(tid.toInteger()));
    return true;
}

void LockManager::unlock(TxId tx, TupleId tid) {
    printf("T%d unlocked %d\n",  int(tx), int(tid.toInteger()));
    TupleBoolMap::accessor a;
    bool lres = locks.find(a, tid);
    assert(lres);
    TupleTxMap::accessor atx;
    bool txres = transactions.find(atx, tid);
    assert(txres);
    atx->second.remove(tx);
    if (a->second || (!a->second && atx->second.size() == 0)) {
        TupleWaiterMap::accessor await;
        transactions.erase(atx);
        locks.erase(a);
        if (waiterlocks.find(await, tid)) {
            await->second.first->notify_one();
        }
    }
}

void LockManager::unlockAll(TxId tx) {
    TxTupleMap::accessor a;
    if (lockedtuples.find(a, tx)) {
        for (TupleId tid : a->second) {
            unlock(tx, tid);
        }
        lockedtuples.erase(a);
    }
}

bool LockManager::deadlock(TxId tx, TupleId tid) {
    //TupleBoolMap::accessor abool;
    //bool lres = locks.find(abool, tid);
    //assert(lres);
    TxTupleMap::const_accessor ac;
    bool res = lockedtuples.find(ac, tx);
    if (!res)
        return false;
    TidList t = ac->second;
    ac.release();
    TupleTxMap::const_accessor a;
    assert(transactions.find(a, tid));
    // Loop through all transactions which have currently locked tid
    for (TxId othertx: a->second) {
        // Check if other transaction waits for something tx has locked
        TxTidMap::const_accessor atup;
        if (waitingfor.find(atup, othertx)) {
            if (std::find(t.cbegin(), t.cend(), atup->second) != t.cend())
               return true;
        }
    }
    return false;
}

}
