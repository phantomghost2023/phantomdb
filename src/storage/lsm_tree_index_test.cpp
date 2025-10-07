#include "index_manager.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::storage;

void testLSMTreeIndexCreation() {
    std::cout << "Testing LSM-tree index creation..." << std::endl;
    
    IndexManager indexManager;
    
    // Create an LSM-tree index
    bool result = indexManager.createIndex("test_table", "test_column", IndexType::LSM_TREE);
    assert(result);
    
    // Check the index type
    std::string indexName = "test_table_test_column_idx";
    IndexType type = indexManager.getIndexType(indexName);
    assert(type == IndexType::LSM_TREE);
    
    std::cout << "LSM-tree index creation test passed!" << std::endl;
}

void testLSMTreeIndexOperations() {
    std::cout << "Testing LSM-tree index operations..." << std::endl;
    
    IndexManager indexManager;
    
    // Create an LSM-tree index
    bool result = indexManager.createIndex("test_table", "test_column", IndexType::LSM_TREE);
    assert(result);
    
    std::string indexName = "test_table_test_column_idx";
    
    // Insert some data
    assert(indexManager.insertIntoIndex(indexName, 1, "value1"));
    assert(indexManager.insertIntoIndex(indexName, 2, "value2"));
    assert(indexManager.insertIntoIndex(indexName, 3, "value3"));
    
    // Search for data
    std::string value;
    assert(indexManager.searchInIndex(indexName, 1, value));
    assert(value == "value1");
    
    assert(indexManager.searchInIndex(indexName, 2, value));
    assert(value == "value2");
    
    assert(indexManager.searchInIndex(indexName, 3, value));
    assert(value == "value3");
    
    // Search for non-existent data
    assert(!indexManager.searchInIndex(indexName, 4, value));
    
    std::cout << "LSM-tree index operations test passed!" << std::endl;
}

void testLSMTreeIndexDrop() {
    std::cout << "Testing LSM-tree index drop..." << std::endl;
    
    IndexManager indexManager;
    
    // Create an LSM-tree index
    bool result = indexManager.createIndex("test_table", "test_column", IndexType::LSM_TREE);
    assert(result);
    
    std::string indexName = "test_table_test_column_idx";
    
    // Insert some data
    assert(indexManager.insertIntoIndex(indexName, 1, "value1"));
    
    // Drop the index
    assert(indexManager.dropIndex(indexName));
    
    // Try to search in dropped index (should fail)
    std::string value;
    assert(!indexManager.searchInIndex(indexName, 1, value));
    
    std::cout << "LSM-tree index drop test passed!" << std::endl;
}

int main() {
    std::cout << "Running LSM-tree index integration tests..." << std::endl;
    
    testLSMTreeIndexCreation();
    testLSMTreeIndexOperations();
    testLSMTreeIndexDrop();
    
    std::cout << "All LSM-tree index integration tests passed!" << std::endl;
    return 0;
}