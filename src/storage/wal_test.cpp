#include "wal_manager.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing WAL Manager..." << std::endl;
    
    phantomdb::storage::WALManager walManager;
    
    // Test initialization
    assert(walManager.initialize());
    std::cout << "Initialization test passed" << std::endl;
    
    // Test writing log entries
    assert(walManager.writeLogEntry("First log entry"));
    assert(walManager.writeLogEntry("Second log entry"));
    assert(walManager.writeLogEntry("Third log entry"));
    std::cout << "Write log entries test passed" << std::endl;
    
    // Test log replay
    assert(walManager.replayLogs());
    std::cout << "Log replay test passed" << std::endl;
    
    std::cout << "All WAL Manager tests passed!" << std::endl;
    return 0;
}