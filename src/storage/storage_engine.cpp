#include "storage_engine.h"
#include "wal_manager.h"
#include "index_manager.h"
#include "garbage_collector.h"
#include <iostream>
#include <memory>

namespace phantomdb {
namespace storage {

class StorageEngine::Impl {
public:
    Impl() : status_("initialized") {}
    ~Impl() = default;
    
    bool initialize() {
        std::cout << "Initializing storage engine components..." << std::endl;
        
        // Initialize WAL manager
        if (!walManager.initialize()) {
            std::cerr << "Failed to initialize WAL manager" << std::endl;
            return false;
        }
        
        // Initialize index manager
        if (!indexManager.initialize()) {
            std::cerr << "Failed to initialize index manager" << std::endl;
            return false;
        }
        
        // Initialize garbage collector
        if (!gc.initialize()) {
            std::cerr << "Failed to initialize garbage collector" << std::endl;
            return false;
        }
        
        // Start garbage collector
        gc.setCollectionInterval(30);
        
        status_ = "running";
        std::cout << "Storage engine initialization completed" << std::endl;
        return true;
    }
    
    void shutdown() {
        std::cout << "Shutting down storage engine components..." << std::endl;
        
        // Shutdown garbage collector
        gc.shutdown();
        
        // Shutdown index manager
        indexManager.shutdown();
        
        // Shutdown WAL manager
        walManager.shutdown();
        
        status_ = "shutdown";
        std::cout << "Storage engine shutdown completed" << std::endl;
    }
    
    bool writeData(const std::string& data) {
        // Write to WAL for durability
        if (!walManager.writeLogEntry(data)) {
            std::cerr << "Failed to write to WAL" << std::endl;
            return false;
        }
        
        // In a real implementation, this would also write to the storage structures
        std::cout << "Data written: " << data << std::endl;
        return true;
    }
    
    std::string status_;
    WALManager walManager;
    IndexManager indexManager;
    GarbageCollector gc;
};

StorageEngine::StorageEngine() : pImpl(std::make_unique<Impl>()) {
    std::cout << "PhantomDB Storage Engine initialized" << std::endl;
}

StorageEngine::~StorageEngine() {
    std::cout << "PhantomDB Storage Engine shutdown" << std::endl;
}

bool StorageEngine::initialize() {
    std::cout << "Initializing PhantomDB Storage Engine" << std::endl;
    return pImpl->initialize();
}

void StorageEngine::shutdown() {
    std::cout << "Shutting down PhantomDB Storage Engine" << std::endl;
    pImpl->shutdown();
}

std::string StorageEngine::getStatus() const {
    return pImpl->status_;
}

bool StorageEngine::writeData(const std::string& data) {
    return pImpl->writeData(data);
}

} // namespace storage
} // namespace phantomdb