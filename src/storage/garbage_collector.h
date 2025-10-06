#ifndef PHANTOMDB_GARBAGE_COLLECTOR_H
#define PHANTOMDB_GARBAGE_COLLECTOR_H

#include <string>
#include <memory>

namespace phantomdb {
namespace storage {

class GarbageCollector {
public:
    GarbageCollector();
    ~GarbageCollector();
    
    // Initialize the garbage collector
    bool initialize();
    
    // Shutdown the garbage collector
    void shutdown();
    
    // Run garbage collection
    bool collectGarbage();
    
    // Set garbage collection interval (in seconds)
    void setCollectionInterval(int seconds);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace storage
} // namespace phantomdb

#endif // PHANTOMDB_GARBAGE_COLLECTOR_H