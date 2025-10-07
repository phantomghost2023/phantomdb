#include "mvcc_manager.h"
#include "transaction_manager.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::transaction;

void testMVCCManagerCreation() {
    std::cout << "Testing MVCCManager creation..." << std::endl;
    
    MVCCManager manager;
    assert(manager.initialize());
    
    std::cout << "MVCCManager creation test passed!" << std::endl;
}

void testCreateVersion() {
    std::cout << "Testing create version..." << std::endl;
    
    MVCCManager manager;
    manager.initialize();
    
    bool result = manager.createVersion(1, "key1", "value1");
    assert(result);
    
    std::cout << "Create version test passed!" << std::endl;
}

void testReadData() {
    std::cout << "Testing read data..." << std::endl;
    
    MVCCManager manager;
    manager.initialize();
    
    // Create a version
    assert(manager.createVersion(1, "key1", "value1"));
    
    // Try to read the data (should fail since it's not committed)
    std::string data;
    bool result = manager.readData(2, "key1", data, IsolationLevel::READ_COMMITTED);
    assert(!result); // Should not be able to read uncommitted data
    
    // Commit the transaction
    assert(manager.commitTransaction(1));
    
    // Now try to read the data (should succeed)
    result = manager.readData(2, "key1", data, IsolationLevel::READ_COMMITTED);
    assert(result);
    assert(data == "value1");
    
    std::cout << "Read data test passed!" << std::endl;
}

void testAbortTransaction() {
    std::cout << "Testing abort transaction..." << std::endl;
    
    MVCCManager manager;
    manager.initialize();
    
    // Create a version
    assert(manager.createVersion(1, "key1", "value1"));
    
    // Abort the transaction
    assert(manager.abortTransaction(1));
    
    // Try to read the data (should fail since transaction was aborted)
    std::string data;
    bool result = manager.readData(2, "key1", data, IsolationLevel::READ_COMMITTED);
    assert(!result);
    
    std::cout << "Abort transaction test passed!" << std::endl;
}

int main() {
    std::cout << "Running MVCCManager tests..." << std::endl;
    
    testMVCCManagerCreation();
    testCreateVersion();
    testReadData();
    testAbortTransaction();
    
    std::cout << "All MVCCManager tests passed!" << std::endl;
    return 0;
}