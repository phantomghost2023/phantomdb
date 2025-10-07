#include "index_manager.h"
#include <iostream>
#include <vector>

using namespace phantomdb::storage;

int main() {
    std::cout << "Testing enhanced Index Manager with automatic indexing..." << std::endl;
    
    // Initialize the index manager
    IndexManager indexManager;
    if (!indexManager.initialize()) {
        std::cerr << "Failed to initialize index manager" << std::endl;
        return 1;
    }
    
    // Test 1: Enable automatic indexing for a table
    std::cout << "\n--- Test 1: Enable automatic indexing ---" << std::endl;
    std::vector<std::string> autoIndexColumns = {"id", "email"};
    indexManager.enableAutoIndexing("users", autoIndexColumns, IndexType::B_TREE);
    
    if (indexManager.isAutoIndexingEnabled("users")) {
        std::cout << "Auto-indexing is enabled for 'users' table" << std::endl;
        std::cout << "Auto-index type: B_TREE" << std::endl;
        
        const auto& columns = indexManager.getAutoIndexColumns("users");
        std::cout << "Auto-index columns: ";
        for (const auto& col : columns) {
            std::cout << col << " ";
        }
        std::cout << std::endl;
    } else {
        std::cerr << "Auto-indexing is not enabled for 'users' table" << std::endl;
        return 1;
    }
    
    // Test 2: Create automatic indexes
    std::cout << "\n--- Test 2: Create automatic indexes ---" << std::endl;
    // This would normally be called when creating a table
    
    // Test 3: Create manual indexes
    std::cout << "\n--- Test 3: Create manual indexes ---" << std::endl;
    if (indexManager.createIndex("users", "name", IndexType::HASH)) {
        std::cout << "Created HASH index on users.name" << std::endl;
    } else {
        std::cerr << "Failed to create HASH index on users.name" << std::endl;
        return 1;
    }
    
    if (indexManager.createIndex("orders", "date", IndexType::LSM_TREE)) {
        std::cout << "Created LSM_TREE index on orders.date" << std::endl;
    } else {
        std::cerr << "Failed to create LSM_TREE index on orders.date" << std::endl;
        return 1;
    }
    
    // Test 4: List all indexes
    std::cout << "\n--- Test 4: List all indexes ---" << std::endl;
    indexManager.listIndexes();
    
    // Test 5: Insert and search in indexes
    std::cout << "\n--- Test 5: Insert and search in indexes ---" << std::endl;
    if (indexManager.insertIntoIndex("users_name_idx", 1, "John Doe")) {
        std::cout << "Inserted key=1, value='John Doe' into users_name_idx" << std::endl;
    } else {
        std::cerr << "Failed to insert into users_name_idx" << std::endl;
        return 1;
    }
    
    std::string searchResult;
    if (indexManager.searchInIndex("users_name_idx", 1, searchResult)) {
        std::cout << "Found key=1 with value='" << searchResult << "'" << std::endl;
    } else {
        std::cerr << "Failed to find key=1 in users_name_idx" << std::endl;
        return 1;
    }
    
    // Test 6: Drop an index
    std::cout << "\n--- Test 6: Drop an index ---" << std::endl;
    if (indexManager.dropIndex("orders_date_idx")) {
        std::cout << "Dropped index orders_date_idx" << std::endl;
    } else {
        std::cerr << "Failed to drop index orders_date_idx" << std::endl;
        return 1;
    }
    
    // Test 7: Disable automatic indexing
    std::cout << "\n--- Test 7: Disable automatic indexing ---" << std::endl;
    indexManager.disableAutoIndexing("users");
    if (!indexManager.isAutoIndexingEnabled("users")) {
        std::cout << "Auto-indexing disabled for 'users' table" << std::endl;
    } else {
        std::cerr << "Auto-indexing is still enabled for 'users' table" << std::endl;
        return 1;
    }
    
    // Shutdown the index manager
    indexManager.shutdown();
    
    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}