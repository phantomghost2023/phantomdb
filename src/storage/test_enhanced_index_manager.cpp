#include "enhanced_index_manager.h"
#include <iostream>
#include <cassert>
#include <vector>

int main() {
    std::cout << "Testing Enhanced Index Manager..." << std::endl;
    
    // Create an index manager instance
    phantomdb::storage::EnhancedIndexManager indexManager;
    
    // Initialize the index manager
    assert(indexManager.initialize());
    
    // Test creating different types of indexes
    std::cout << "\n--- Testing Index Creation ---" << std::endl;
    
    // Create a B-tree index
    phantomdb::storage::IndexConfig btreeConfig;
    btreeConfig.cacheSize = 2000;
    btreeConfig.useCompression = true;
    assert(indexManager.createIndex("users", "id", phantomdb::storage::IndexType::B_TREE, btreeConfig));
    
    // Create a Hash index
    phantomdb::storage::IndexConfig hashConfig;
    hashConfig.cacheSize = 1500;
    assert(indexManager.createIndex("users", "email", phantomdb::storage::IndexType::HASH, hashConfig));
    
    // Create an LSM-tree index
    phantomdb::storage::IndexConfig lsmConfig;
    lsmConfig.cacheSize = 3000;
    assert(indexManager.createIndex("logs", "timestamp", phantomdb::storage::IndexType::LSM_TREE, lsmConfig));
    
    // Test listing indexes
    std::cout << "\n--- Current Indexes ---" << std::endl;
    indexManager.listIndexes();
    
    // Test inserting data into indexes
    std::cout << "\n--- Testing Data Insertion ---" << std::endl;
    
    // Insert data into B-tree index
    assert(indexManager.insertIntoIndex("users_id_idx", "1001", "John Doe"));
    assert(indexManager.insertIntoIndex("users_id_idx", "1002", "Jane Smith"));
    assert(indexManager.insertIntoIndex("users_id_idx", "1003", "Bob Johnson"));
    
    // Insert data into Hash index
    assert(indexManager.insertIntoIndex("users_email_idx", "john@example.com", "1001"));
    assert(indexManager.insertIntoIndex("users_email_idx", "jane@example.com", "1002"));
    assert(indexManager.insertIntoIndex("users_email_idx", "bob@example.com", "1003"));
    
    // Insert data into LSM-tree index
    assert(indexManager.insertIntoIndex("logs_timestamp_idx", "2023-12-01T10:00:00Z", "Log entry 1"));
    assert(indexManager.insertIntoIndex("logs_timestamp_idx", "2023-12-01T11:00:00Z", "Log entry 2"));
    
    // Test searching data in indexes
    std::cout << "\n--- Testing Data Search ---" << std::endl;
    
    // Search in B-tree index
    std::string value;
    assert(indexManager.searchInIndex("users_id_idx", "1001", value));
    assert(value == "John Doe");
    std::cout << "Found user with ID 1001: " << value << std::endl;
    
    // Search in Hash index
    assert(indexManager.searchInIndex("users_email_idx", "jane@example.com", value));
    assert(value == "1002");
    std::cout << "Found user with email jane@example.com, ID: " << value << std::endl;
    
    // Search in LSM-tree index
    assert(indexManager.searchInIndex("logs_timestamp_idx", "2023-12-01T10:00:00Z", value));
    assert(value == "Log entry 1");
    std::cout << "Found log entry: " << value << std::endl;
    
    // Test range search (B-tree only)
    std::cout << "\n--- Testing Range Search ---" << std::endl;
    std::vector<std::pair<std::string, std::string>> rangeResults;
    assert(indexManager.rangeSearch("users_id_idx", "1001", "1003", rangeResults));
    std::cout << "Range search completed successfully" << std::endl;
    
    // Test bulk insert
    std::cout << "\n--- Testing Bulk Insert ---" << std::endl;
    std::vector<std::pair<std::string, std::string>> bulkData = {
        {"1004", "Alice Brown"},
        {"1005", "Charlie Wilson"},
        {"1006", "Diana Lee"}
    };
    assert(indexManager.bulkInsert("users_id_idx", bulkData));
    std::cout << "Bulk insert of 3 items completed successfully" << std::endl;
    
    // Test index statistics
    std::cout << "\n--- Testing Index Statistics ---" << std::endl;
    auto stats = indexManager.getIndexStats("users_id_idx");
    std::cout << "Index: " << stats.indexName << std::endl;
    std::cout << "Type: " << (stats.type == phantomdb::storage::IndexType::B_TREE ? "B-tree" : "Other") << std::endl;
    std::cout << "Key count: " << stats.keyCount << std::endl;
    std::cout << "Average lookup time: " << stats.avgLookupTime << " microseconds" << std::endl;
    std::cout << "Average insert time: " << stats.avgInsertTime << " microseconds" << std::endl;
    
    // Test index configuration
    std::cout << "\n--- Testing Index Configuration ---" << std::endl;
    auto config = indexManager.getIndexConfig("users_id_idx");
    std::cout << "Cache size: " << config.cacheSize << std::endl;
    std::cout << "Compression enabled: " << (config.useCompression ? "Yes" : "No") << std::endl;
    
    // Test updating index configuration
    phantomdb::storage::IndexConfig newConfig = config;
    newConfig.cacheSize = 5000;
    assert(indexManager.updateIndexConfig("users_id_idx", newConfig));
    std::cout << "Updated cache size to 5000" << std::endl;
    
    // Test auto-indexing
    std::cout << "\n--- Testing Auto-Indexing ---" << std::endl;
    std::vector<std::string> autoColumns = {"name", "age"};
    phantomdb::storage::IndexConfig autoConfig;
    autoConfig.cacheSize = 1000;
    indexManager.enableAutoIndexing("products", autoColumns, phantomdb::storage::IndexType::HASH, autoConfig);
    assert(indexManager.isAutoIndexingEnabled("products"));
    std::cout << "Auto-indexing enabled for products table" << std::endl;
    
    // Test index analysis
    std::cout << "\n--- Testing Index Analysis ---" << std::endl;
    indexManager.analyzeIndex("users_id_idx");
    
    // Test index operations
    std::cout << "\n--- Testing Index Operations ---" << std::endl;
    assert(indexManager.flushIndex("users_id_idx"));
    assert(indexManager.loadIndex("users_id_idx"));
    assert(indexManager.rebuildIndex("users_id_idx"));
    std::cout << "Index operations completed successfully" << std::endl;
    
    // Test deleting data from indexes
    std::cout << "\n--- Testing Data Deletion ---" << std::endl;
    assert(indexManager.deleteFromIndex("users_id_idx", "1001"));
    std::cout << "Deleted user with ID 1001" << std::endl;
    
    // Verify deletion
    assert(!indexManager.searchInIndex("users_id_idx", "1001", value));
    std::cout << "Verified user with ID 1001 no longer exists" << std::endl;
    
    // Test dropping indexes
    std::cout << "\n--- Testing Index Deletion ---" << std::endl;
    assert(indexManager.dropIndex("logs_timestamp_idx"));
    std::cout << "Dropped LSM-tree index successfully" << std::endl;
    
    // Shutdown the index manager
    indexManager.shutdown();
    
    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}