#include "storage_engine.h"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>

int main() {
    std::cout << "Testing Storage Engine Integration..." << std::endl;
    
    phantomdb::storage::StorageEngine engine;
    
    // Test initialization
    assert(engine.initialize());
    assert(engine.getStatus() == "running");
    std::cout << "Initialization test passed" << std::endl;
    
    // Test writing data
    assert(engine.writeData("First data entry"));
    assert(engine.writeData("Second data entry"));
    assert(engine.writeData("Third data entry"));
    std::cout << "Data writing test passed" << std::endl;
    
    // Let garbage collector run for a bit
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Test shutdown
    engine.shutdown();
    assert(engine.getStatus() == "shutdown");
    std::cout << "Shutdown test passed" << std::endl;
    
    std::cout << "All Storage Engine Integration tests passed!" << std::endl;
    return 0;
}