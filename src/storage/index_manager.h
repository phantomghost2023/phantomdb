#ifndef PHANTOMDB_INDEX_MANAGER_H
#define PHANTOMDB_INDEX_MANAGER_H

#include <string>
#include <memory>

namespace phantomdb {
namespace storage {

class IndexManager {
public:
    IndexManager();
    ~IndexManager();
    
    // Initialize the index manager
    bool initialize();
    
    // Shutdown the index manager
    void shutdown();
    
    // Create an index
    bool createIndex(const std::string& tableName, const std::string& columnName);
    
    // Drop an index
    bool dropIndex(const std::string& indexName);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace storage
} // namespace phantomdb

#endif // PHANTOMDB_INDEX_MANAGER_H