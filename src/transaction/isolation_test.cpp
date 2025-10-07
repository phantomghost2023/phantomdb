#include "isolation_manager.h"
#include "mvcc_manager.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::transaction;

void testIsolationManagerCreation() {
    std::cout << "Testing IsolationManager creation..." << std::endl;
    
    IsolationManager manager;
    assert(manager.initialize());
    
    std::cout << "IsolationManager creation test passed!" << std::endl;
}

void testReadAllowed() {
    std::cout << "Testing read allowed..." << std::endl;
    
    IsolationManager manager;
    manager.initialize();
    
    // All isolation levels should allow reads
    assert(manager.isReadAllowed(IsolationLevel::READ_UNCOMMITTED, "key1"));
    assert(manager.isReadAllowed(IsolationLevel::READ_COMMITTED, "key1"));
    assert(manager.isReadAllowed(IsolationLevel::REPEATABLE_READ, "key1"));
    assert(manager.isReadAllowed(IsolationLevel::SERIALIZABLE, "key1"));
    assert(manager.isReadAllowed(IsolationLevel::SNAPSHOT, "key1"));
    
    std::cout << "Read allowed test passed!" << std::endl;
}

void testWriteAllowed() {
    std::cout << "Testing write allowed..." << std::endl;
    
    IsolationManager manager;
    manager.initialize();
    
    // All isolation levels should allow writes
    assert(manager.isWriteAllowed(IsolationLevel::READ_UNCOMMITTED, "key1"));
    assert(manager.isWriteAllowed(IsolationLevel::READ_COMMITTED, "key1"));
    assert(manager.isWriteAllowed(IsolationLevel::REPEATABLE_READ, "key1"));
    assert(manager.isWriteAllowed(IsolationLevel::SERIALIZABLE, "key1"));
    assert(manager.isWriteAllowed(IsolationLevel::SNAPSHOT, "key1"));
    
    std::cout << "Write allowed test passed!" << std::endl;
}

void testIsVisible() {
    std::cout << "Testing visibility..." << std::endl;
    
    IsolationManager manager;
    manager.initialize();
    
    // Create a committed version
    Timestamp timestamp = std::chrono::high_resolution_clock::now();
    DataVersion committedVersion(1, timestamp, "data", true);
    
    // Create an uncommitted version
    DataVersion uncommittedVersion(2, timestamp, "data", false);
    
    // READ_UNCOMMITTED should see all versions
    assert(manager.isVisible(IsolationLevel::READ_UNCOMMITTED, 3, committedVersion));
    assert(manager.isVisible(IsolationLevel::READ_UNCOMMITTED, 3, uncommittedVersion));
    
    // Other isolation levels should only see committed versions
    assert(manager.isVisible(IsolationLevel::READ_COMMITTED, 3, committedVersion));
    assert(!manager.isVisible(IsolationLevel::READ_COMMITTED, 3, uncommittedVersion));
    
    assert(manager.isVisible(IsolationLevel::REPEATABLE_READ, 3, committedVersion));
    assert(!manager.isVisible(IsolationLevel::REPEATABLE_READ, 3, uncommittedVersion));
    
    assert(manager.isVisible(IsolationLevel::SERIALIZABLE, 3, committedVersion));
    assert(!manager.isVisible(IsolationLevel::SERIALIZABLE, 3, uncommittedVersion));
    
    assert(manager.isVisible(IsolationLevel::SNAPSHOT, 3, committedVersion));
    assert(!manager.isVisible(IsolationLevel::SNAPSHOT, 3, uncommittedVersion));
    
    std::cout << "Visibility test passed!" << std::endl;
}

void testPreventPhantomReads() {
    std::cout << "Testing phantom read prevention..." << std::endl;
    
    IsolationManager manager;
    manager.initialize();
    
    // SERIALIZABLE should track reads
    assert(manager.preventPhantomReads(IsolationLevel::SERIALIZABLE, 1, "key1"));
    
    // Other isolation levels should not need tracking
    assert(manager.preventPhantomReads(IsolationLevel::READ_COMMITTED, 1, "key1"));
    
    std::cout << "Phantom read prevention test passed!" << std::endl;
}

int main() {
    std::cout << "Running IsolationManager tests..." << std::endl;
    
    testIsolationManagerCreation();
    testReadAllowed();
    testWriteAllowed();
    testIsVisible();
    testPreventPhantomReads();
    
    std::cout << "All IsolationManager tests passed!" << std::endl;
    return 0;
}