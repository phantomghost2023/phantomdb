#ifndef PHANTOMDB_INDEX_MANAGER_H
#define PHANTOMDB_INDEX_MANAGER_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace phantomdb {
namespace storage {

enum class IndexType {
    B_TREE,
    HASH,
    LSM_TREE
};

class IndexManager {
public:
    IndexManager();
    ~IndexManager();
    
    // Initialize the index manager
    bool initialize();
    
    // Shutdown the index manager
    void shutdown();
    
    // Create an index
    bool createIndex(const std::string& tableName, const std::string& columnName, IndexType type = IndexType::B_TREE);
    
    // Drop an index
    bool dropIndex(const std::string& indexName);
    
    // Get index type
    IndexType getIndexType(const std::string& indexName) const;
    
    // Insert a key-value pair into an index
    bool insertIntoIndex(const std::string& indexName, int key, const std::string& value);
    
    // Search for a key in an index
    bool searchInIndex(const std::string& indexName, int key, std::string& value) const;
    
    // List all indexes
    void listIndexes() const;
    
    // Enable automatic indexing for a table
    void enableAutoIndexing(const std::string& tableName, const std::vector<std::string>& columns, IndexType type = IndexType::B_TREE);
    
    // Disable automatic indexing for a table
    void disableAutoIndexing(const std::string& tableName);
    
    // Check if a table has automatic indexing enabled
    bool isAutoIndexingEnabled(const std::string& tableName) const;
    
    // Get the index type for automatic indexing
    IndexType getAutoIndexType(const std::string& tableName) const;
    
    // Get columns that should be automatically indexed
    const std::vector<std::string>& getAutoIndexColumns(const std::string& tableName) const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace storage
} // namespace phantomdb

#endif // PHANTOMDB_INDEX_MANAGER_H