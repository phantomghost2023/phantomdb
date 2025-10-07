#include <iostream>
#include "core/core.h"
#include "storage/storage_engine.h"

int main() {
    std::cout << "Starting PhantomDB..." << std::endl;
    
    // Initialize core components
    phantomdb::core::Core core;
    if (!core.initialize()) {
        std::cout << "Failed to initialize core components" << std::endl;
        return 1;
    }
    
    // Initialize storage engine
    phantomdb::storage::StorageEngine storage;
    if (!storage.initialize()) {
        std::cout << "Failed to initialize storage engine" << std::endl;
        return 1;
    }
    
    std::cout << "PhantomDB version: " << core.getVersion() << std::endl;
    std::cout << "Storage engine status: " << storage.getStatus() << std::endl;
    
    std::cout << "PhantomDB started successfully!" << std::endl;
    
    // Shutdown components
    storage.shutdown();
    core.shutdown();
    
    return 0;
}