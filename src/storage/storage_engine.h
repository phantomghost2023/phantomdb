#ifndef PHANTOMDB_STORAGE_ENGINE_H
#define PHANTOMDB_STORAGE_ENGINE_H

#include <string>
#include <memory>

namespace phantomdb {
namespace storage {

class StorageEngine {
public:
    StorageEngine();
    ~StorageEngine();
    
    // Initialize the storage engine
    bool initialize();
    
    // Shutdown the storage engine
    void shutdown();
    
    // Get storage engine status
    std::string getStatus() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace storage
} // namespace phantomdb

#endif // PHANTOMDB_STORAGE_ENGINE_H