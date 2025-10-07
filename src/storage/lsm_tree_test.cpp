#include "lsm_tree.h"
#include <iostream>
#include <cassert>
#include <string>

using namespace phantomdb::storage;

void testBasicOperations() {
    std::cout << "Testing basic LSM-tree operations..." << std::endl;
    
    LSMTREE<int, std::string> lsmTree(5); // Small memtable size for testing
    
    // Test insertion
    lsmTree.insert(1, "value1");
    lsmTree.insert(2, "value2");
    lsmTree.insert(3, "value3");
    
    // Test search
    std::string value;
    assert(lsmTree.search(1, value));
    assert(value == "value1");
    
    assert(lsmTree.search(2, value));
    assert(value == "value2");
    
    assert(lsmTree.search(3, value));
    assert(value == "value3");
    
    // Test non-existent key
    assert(!lsmTree.search(4, value));
    
    std::cout << "Basic operations test passed!" << std::endl;
}

void testMemtableFlush() {
    std::cout << "Testing memtable flush..." << std::endl;
    
    LSMTREE<int, std::string> lsmTree(3); // Very small memtable size
    
    // Insert more items than memtable can hold to trigger flush
    for (int i = 0; i < 10; ++i) {
        lsmTree.insert(i, "value" + std::to_string(i));
    }
    
    // All items should still be searchable
    for (int i = 0; i < 10; ++i) {
        std::string value;
        assert(lsmTree.search(i, value));
        assert(value == "value" + std::to_string(i));
    }
    
    std::cout << "Memtable flush test passed!" << std::endl;
}

void testUpdate() {
    std::cout << "Testing update operations..." << std::endl;
    
    LSMTREE<int, std::string> lsmTree;
    
    // Insert initial values
    lsmTree.insert(1, "initial_value");
    
    // Update value
    lsmTree.insert(1, "updated_value");
    
    // Check that the value was updated
    std::string value;
    assert(lsmTree.search(1, value));
    assert(value == "updated_value");
    
    std::cout << "Update operations test passed!" << std::endl;
}

void testSizeAndCount() {
    std::cout << "Testing size and count..." << std::endl;
    
    LSMTREE<int, std::string> lsmTree;
    
    assert(lsmTree.getSize() == 0);
    assert(lsmTree.getCount() == 0);
    
    // Add some items
    lsmTree.insert(1, "value1");
    lsmTree.insert(2, "value2");
    
    assert(lsmTree.getCount() == 2);
    
    // Update an item (should not change count)
    lsmTree.insert(1, "updated_value1");
    assert(lsmTree.getCount() == 2);
    
    std::cout << "Size and count test passed!" << std::endl;
}

int main() {
    std::cout << "Running LSM-tree tests..." << std::endl;
    
    testBasicOperations();
    testMemtableFlush();
    testUpdate();
    testSizeAndCount();
    
    std::cout << "All LSM-tree tests passed!" << std::endl;
    return 0;
}