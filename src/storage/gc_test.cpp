#include "garbage_collector.h"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>

int main() {
    std::cout << "Testing Garbage Collector..." << std::endl;
    
    phantomdb::storage::GarbageCollector gc;
    
    // Test initialization
    assert(gc.initialize());
    std::cout << "Initialization test passed" << std::endl;
    
    // Test setting collection interval
    gc.setCollectionInterval(5);
    std::cout << "Set collection interval test passed" << std::endl;
    
    // Test manual garbage collection
    assert(gc.collectGarbage());
    std::cout << "Manual garbage collection test passed" << std::endl;
    
    std::cout << "All Garbage Collector tests passed!" << std::endl;
    return 0;
}