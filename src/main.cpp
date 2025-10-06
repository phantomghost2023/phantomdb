#include <iostream>
#include "core/core.h"
#include "storage/storage_engine.h"
#include <spdlog/spdlog.h>

int main() {
    spdlog::info("Starting PhantomDB...");
    
    // Initialize core components
    phantomdb::core::Core core;
    if (!core.initialize()) {
        spdlog::error("Failed to initialize core components");
        return 1;
    }
    
    // Initialize storage engine
    phantomdb::storage::StorageEngine storage;
    if (!storage.initialize()) {
        spdlog::error("Failed to initialize storage engine");
        return 1;
    }
    
    spdlog::info("PhantomDB version: {}", core.getVersion());
    spdlog::info("Storage engine status: {}", storage.getStatus());
    
    spdlog::info("PhantomDB started successfully!");
    
    // Shutdown components
    storage.shutdown();
    core.shutdown();
    
    return 0;
}