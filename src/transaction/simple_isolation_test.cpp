#include "isolation_manager.h"
#include "mvcc_manager.h"
#include <iostream>

using namespace phantomdb::transaction;

int main() {
    std::cout << "Testing Isolation Manager compilation..." << std::endl;
    
    // Create an IsolationManager instance
    IsolationManager isolationManager;
    if (!isolationManager.initialize()) {
        std::cout << "Failed to initialize IsolationManager!" << std::endl;
        return 1;
    }
    
    // Create a MVCCManager instance
    MVCCManager mvccManager;
    if (!mvccManager.initialize()) {
        std::cout << "Failed to initialize MVCCManager!" << std::endl;
        return 1;
    }
    
    // Test basic functionality
    bool readAllowed = isolationManager.isReadAllowed(IsolationLevel::READ_COMMITTED, "key1");
    if (!readAllowed) {
        std::cout << "Read operation not allowed!" << std::endl;
        return 1;
    }
    
    std::cout << "Isolation Manager compilation test passed!" << std::endl;
    return 0;
}