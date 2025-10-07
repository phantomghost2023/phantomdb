#include "btree.h"
#include <iostream>
#include <cassert>
#include <string>

int main() {
    std::cout << "Testing B-tree implementation..." << std::endl;
    
    // Create a B-tree with integer keys and string values
    phantomdb::storage::BTree<int, std::string> btree;
    
    // Test insertion
    std::cout << "Inserting key-value pairs..." << std::endl;
    btree.insert(1, "one");
    btree.insert(2, "two");
    btree.insert(3, "three");
    btree.insert(4, "four");
    btree.insert(5, "five");
    
    // Test search
    std::cout << "Testing search..." << std::endl;
    std::string value;
    assert(btree.search(1, value) && value == "one");
    assert(btree.search(3, value) && value == "three");
    assert(btree.search(5, value) && value == "five");
    assert(!btree.search(10, value)); // Key doesn't exist
    
    std::cout << "Search tests passed" << std::endl;
    
    // Test removal
    std::cout << "Testing removal..." << std::endl;
    assert(btree.remove(3));
    assert(!btree.search(3, value)); // Key should no longer exist
    assert(btree.search(1, value) && value == "one"); // Other keys should still exist
    assert(btree.search(5, value) && value == "five");
    
    std::cout << "Removal tests passed" << std::endl;
    
    // Test with more insertions to trigger node splits
    std::cout << "Testing with more insertions..." << std::endl;
    for (int i = 10; i < 50; i++) {
        btree.insert(i, "value" + std::to_string(i));
    }
    
    // Verify some of the inserted values
    assert(btree.search(15, value) && value == "value15");
    assert(btree.search(30, value) && value == "value30");
    assert(btree.search(45, value) && value == "value45");
    
    std::cout << "Extended insertion tests passed" << std::endl;
    
    // Print the tree structure (for debugging)
    std::cout << "B-tree structure:" << std::endl;
    btree.print();
    
    std::cout << "All B-tree tests passed!" << std::endl;
    return 0;
}