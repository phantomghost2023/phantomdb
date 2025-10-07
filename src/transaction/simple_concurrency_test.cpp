#include "mvcc_manager.h"
#include "lock_manager.h"
#include <iostream>

using namespace phantomdb::transaction;

int main() {
    std::cout << "Testing MVCC and Lock Manager compilation..." << std::endl;
    
    // Create an MVCCManager instance
    MVCCManager mvccManager;
    if (!mvccManager.initialize()) {
        std::cout << "Failed to initialize MVCCManager!" << std::endl;
        return 1;
    }
    
    // Create a LockManager instance
    LockManager lockManager;
    if (!lockManager.initialize()) {
        std::cout << "Failed to initialize LockManager!" << std::endl;
        return 1;
    }
    
    // Test basic operations
    if (!mvccManager.createVersion(1, "key1", "value1")) {
        std::cout << "Failed to create version!" << std::endl;
        return 1;
    }
    
    if (!lockManager.acquireLock(1, "resource1", LockType::SHARED)) {
        std::cout << "Failed to acquire lock!" << std::endl;
        return 1;
    }
    
    std::cout << "MVCC and Lock Manager compilation test passed!" << std::endl;
    return 0;
}