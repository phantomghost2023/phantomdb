#include "lock_manager.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::transaction;

void testLockManagerCreation() {
    std::cout << "Testing LockManager creation..." << std::endl;
    
    LockManager manager;
    assert(manager.initialize());
    
    std::cout << "LockManager creation test passed!" << std::endl;
}

void testAcquireSharedLock() {
    std::cout << "Testing acquire shared lock..." << std::endl;
    
    LockManager manager;
    manager.initialize();
    
    bool result = manager.acquireLock(1, "resource1", LockType::SHARED);
    assert(result);
    
    std::cout << "Acquire shared lock test passed!" << std::endl;
}

void testAcquireExclusiveLock() {
    std::cout << "Testing acquire exclusive lock..." << std::endl;
    
    LockManager manager;
    manager.initialize();
    
    bool result = manager.acquireLock(1, "resource1", LockType::EXCLUSIVE);
    assert(result);
    
    std::cout << "Acquire exclusive lock test passed!" << std::endl;
}

void testReleaseLock() {
    std::cout << "Testing release lock..." << std::endl;
    
    LockManager manager;
    manager.initialize();
    
    // Acquire a lock
    assert(manager.acquireLock(1, "resource1", LockType::SHARED));
    
    // Release the lock
    bool result = manager.releaseLock(1, "resource1");
    assert(result);
    
    std::cout << "Release lock test passed!" << std::endl;
}

void testLockConflict() {
    std::cout << "Testing lock conflict..." << std::endl;
    
    LockManager manager;
    manager.initialize();
    
    // Acquire an exclusive lock
    assert(manager.acquireLock(1, "resource1", LockType::EXCLUSIVE));
    
    // Try to acquire a conflicting lock (should fail)
    bool result = manager.acquireLock(2, "resource1", LockType::SHARED);
    assert(!result); // Should fail due to conflict
    
    std::cout << "Lock conflict test passed!" << std::endl;
}

int main() {
    std::cout << "Running LockManager tests..." << std::endl;
    
    testLockManagerCreation();
    testAcquireSharedLock();
    testAcquireExclusiveLock();
    testReleaseLock();
    testLockConflict();
    
    std::cout << "All LockManager tests passed!" << std::endl;
    return 0;
}