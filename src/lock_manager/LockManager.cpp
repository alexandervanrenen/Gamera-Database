
#include <cassert>
#include "LockManager.hpp"

namespace dbi {



bool LockManager::tryLock(TupleId tid, TxId tx, bool exclusive) {
    TupleBoolMap::accessor a;
    bool res = locks.insert(a, {tid, exclusive});
    if (res) {
        TupleTxMap::accessor atx;
        transactions.insert(atx, tid);
        atx->second.push_back(tx);
        TxTupleMap::accessor atup;
        lockedtuples.insert(atup, tx);
        atup->second.push_back(tid);
        return true;
    } else if (!exclusive && !a->second) {
        TupleTxMap::accessor atx;
        transactions.insert(atx, tid);
        atx->second.push_back(tx);
        TxTupleMap::accessor atup;
        lockedtuples.insert(atup, tx);
        atup->second.push_back(tid);
        return true;
    } else {
        TupleTxMap::const_accessor atx;
        if (transactions.find(atx, tid)) {
            if (std::find(atx->second.cbegin(), atx->second.cend(), tx) != atx->second.cend())
                throw AlreadyLockedException();
        }
    }
    return false;
}

bool LockManager::lock(TupleId tid, TxId tx, bool exclusive) {
    bool first = true;
    while (true) {
        TupleBoolMap::accessor a;
        bool res = locks.insert(a, {tid, exclusive});
        
        // Make sure no transaction tries to lock a tuple a second time
        TupleTxMap::const_accessor catx;
        if (transactions.find(catx, tid)) {
            if (std::find(catx->second.cbegin(), catx->second.cend(), tx) != catx->second.cend()) {
                // Already locked and no upgrade
                if (a->second) return false;
                else if (!a->second && !exclusive) return false;
            }
        }
        catx.release();

        // Tuple not yet locked or only shared locked
        if (res || (!exclusive && !a->second)) {
            TupleTxMap::accessor atx;
            transactions.insert(atx, tid);
            atx->second.push_back(tx);
            TxTupleMap::accessor atup;
            lockedtuples.insert(atup, tx);
            atup->second.push_back(tid);
            break;
        }
        
        // Transaction wants lock upgrade from shared to exclusive
        if (!res && !a->second && exclusive) {
            TupleTxMap::accessor atrans;
            // Transaction is the only one to hold the shared lock -> upgrade possible
            if (transactions.find(atrans, tid) && atrans->second.size() == 1) {
                a->second = exclusive;
                return true;
            }
        }

        // Tuple is locked, wait
        if (deadlock(tx, tid))
            throw DeadlockException(); 
        if (first) { // Enter transaction into waiterlist
            TupleTxMap::accessor atup;
            waiterlist.insert(atup, tid);
            atup->second.push_back(tx);
            first = false;
        }
        TupleWaiterMap::accessor await;
        if (waiterlocks.insert(await, tid)) {
            await->second.first = new std::condition_variable;
            await->second.second = new std::mutex;
        }
        std::unique_lock<std::mutex> ul(*await->second.second);
        std::condition_variable* cv = await->second.first;
        await.release();
        a.release();
        cv->wait(ul);
    }
    // Transaction has waited a minimum of one round
    if (!first) {
        waitgraph.clearNode(tx);
        TupleTxMap::accessor atup;
        waiterlist.find(atup, tid);
        atup->second.remove(tx);
    }
    //printf("T%d locked %d\n",  int(tx), int(tid.toInteger()));
    return true;
}

void LockManager::unlock(TxId tx, TupleId tid) {
    //printf("T%d unlocked %d\n",  int(tx), int(tid.toInteger()));
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
    TupleTxMap::const_accessor a;
    assert(transactions.find(a, tid));
    TxList tlist = a->second;
    a.release();
    for (TxId t : tlist) {
        waitgraph.addEdge(tx, t);
    }
    if (waitgraph.hasCycle(tx)) {
        waitgraph.clearNode(tx);
        return true;
    } else 
        return false;

}

}
