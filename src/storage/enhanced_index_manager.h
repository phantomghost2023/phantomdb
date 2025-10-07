#ifndef PHANTOMDB_ENHANCED_INDEX_MANAGER_H
#define PHANTOMDB_ENHANCED_INDEX_MANAGER_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

namespace phantomdb {
namespace storage {

// Enhanced index types with performance characteristics
enum class IndexType {
    B_TREE,      // Balanced tree, good for range queries
    HASH,        // Hash table, excellent for exact match queries
    LSM_TREE,    // Log-structured merge-tree, good for write-heavy workloads
    BITMAP,      // Bitmap index, good for low-cardinality columns
    FULLTEXT     // Full-text search index
};

// Index statistics for performance monitoring
struct IndexStats {
    std::string indexName;
    IndexType type;
    size_t keyCount;
    size_t memoryUsage;
    size_t diskUsage;
    double avgLookupTime;
    double avgInsertTime;
    double avgDeleteTime;
    size_t cacheHits;
    size_t cacheMisses;
};

// Index configuration for optimization
struct IndexConfig {
    bool enabled = true;
    size_t cacheSize = 1000;  // Number of entries to cache
    bool useCompression = false;
    bool allowDuplicates = false;
    size_t maxKeySize = 1024;
    size_t maxValueSize = 8192;
};

class EnhancedIndexManager {
public:
    EnhancedIndexManager();
    ~EnhancedIndexManager();
    
    // Initialize the index manager
    bool initialize();
    
    // Shutdown the index manager
    void shutdown();
    
    // Create an index with enhanced options
    bool createIndex(const std::string& tableName, const std::string& columnName, 
                    IndexType type = IndexType::B_TREE, const IndexConfig& config = IndexConfig{});
    
    // Drop an index
    bool dropIndex(const std::string& indexName);
    
    // Get index type
    IndexType getIndexType(const std::string& indexName) const;
    
    // Insert a key-value pair into an index
    bool insertIntoIndex(const std::string& indexName, const std::string& key, const std::string& value);
    
    // Search for a key in an index
    bool searchInIndex(const std::string& indexName, const std::string& key, std::string& value) const;
    
    // Range search for B-tree indexes
    bool rangeSearch(const std::string& indexName, const std::string& startKey, const std::string& endKey,
                    std::vector<std::pair<std::string, std::string>>& results) const;
    
    // Delete a key from an index
    bool deleteFromIndex(const std::string& indexName, const std::string& key);
    
    // Bulk insert for better performance
    bool bulkInsert(const std::string& indexName, 
                   const std::vector<std::pair<std::string, std::string>>& keyValuePairs);
    
    // List all indexes
    void listIndexes() const;
    
    // Get index statistics
    IndexStats getIndexStats(const std::string& indexName) const;
    
    // Enable automatic indexing for a table
    void enableAutoIndexing(const std::string& tableName, const std::vector<std::string>& columns, 
                           IndexType type = IndexType::B_TREE, const IndexConfig& config = IndexConfig{});
    
    // Disable automatic indexing for a table
    void disableAutoIndexing(const std::string& tableName);
    
    // Check if a table has automatic indexing enabled
    bool isAutoIndexingEnabled(const std::string& tableName) const;
    
    // Get the index type for automatic indexing
    IndexType getAutoIndexType(const std::string& tableName) const;
    
    // Get columns that should be automatically indexed
    const std::vector<std::string>& getAutoIndexColumns(const std::string& tableName) const;
    
    // Get index configuration
    IndexConfig getIndexConfig(const std::string& indexName) const;
    
    // Update index configuration
    bool updateIndexConfig(const std::string& indexName, const IndexConfig& config);
    
    // Flush index to persistent storage
    bool flushIndex(const std::string& indexName);
    
    // Load index from persistent storage
    bool loadIndex(const std::string& indexName);
    
    // Rebuild index for better performance
    bool rebuildIndex(const std::string& indexName);
    
    // Analyze index for optimization suggestions
    void analyzeIndex(const std::string& indexName);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace storage
} // namespace phantomdb

#endif // PHANTOMDB_ENHANCED_INDEX_MANAGER_H