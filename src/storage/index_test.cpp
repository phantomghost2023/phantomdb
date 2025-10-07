#include "index_manager.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Index Manager..." << std::endl;
    
    phantomdb::storage::IndexManager indexManager;
    
    // Test initialization
    assert(indexManager.initialize());
    std::cout << "Initialization test passed" << std::endl;
    
    // Test creating different types of indexes
    assert(indexManager.createIndex("users", "id", phantomdb::storage::IndexType::B_TREE));
    assert(indexManager.createIndex("users", "email", phantomdb::storage::IndexType::HASH));
    assert(indexManager.createIndex("orders", "user_id", phantomdb::storage::IndexType::LSM_TREE));
    std::cout << "Create index tests passed" << std::endl;
    
    // Test getting index types
    assert(indexManager.getIndexType("users_id_idx") == phantomdb::storage::IndexType::B_TREE);
    assert(indexManager.getIndexType("users_email_idx") == phantomdb::storage::IndexType::HASH);
    assert(indexManager.getIndexType("orders_user_id_idx") == phantomdb::storage::IndexType::LSM_TREE);
    std::cout << "Get index type tests passed" << std::endl;
    
    // Test inserting into B-tree index
    assert(indexManager.insertIntoIndex("users_id_idx", 1, "Alice"));
    assert(indexManager.insertIntoIndex("users_id_idx", 2, "Bob"));
    assert(indexManager.insertIntoIndex("users_id_idx", 3, "Charlie"));
    std::cout << "Insert into B-tree index tests passed" << std::endl;
    
    // Test searching in B-tree index
    std::string value;
    assert(indexManager.searchInIndex("users_id_idx", 1, value) && value == "Alice");
    assert(indexManager.searchInIndex("users_id_idx", 2, value) && value == "Bob");
    assert(indexManager.searchInIndex("users_id_idx", 3, value) && value == "Charlie");
    assert(!indexManager.searchInIndex("users_id_idx", 4, value)); // Key doesn't exist
    std::cout << "Search in B-tree index tests passed" << std::endl;
    
    // Test inserting into Hash index
    assert(indexManager.insertIntoIndex("users_email_idx", 1, "alice@example.com"));
    assert(indexManager.insertIntoIndex("users_email_idx", 2, "bob@example.com"));
    assert(indexManager.insertIntoIndex("users_email_idx", 3, "charlie@example.com"));
    std::cout << "Insert into Hash index tests passed" << std::endl;
    
    // Test searching in Hash index
    assert(indexManager.searchInIndex("users_email_idx", 1, value) && value == "alice@example.com");
    assert(indexManager.searchInIndex("users_email_idx", 2, value) && value == "bob@example.com");
    assert(indexManager.searchInIndex("users_email_idx", 3, value) && value == "charlie@example.com");
    assert(!indexManager.searchInIndex("users_email_idx", 4, value)); // Key doesn't exist
    std::cout << "Search in Hash index tests passed" << std::endl;
    
    // Test dropping an index
    assert(indexManager.dropIndex("users_email_idx"));
    std::cout << "Drop index test passed" << std::endl;
    
    std::cout << "All Index Manager tests passed!" << std::endl;
    return 0;
}