#include "hash_table.h"
#include <iostream>
#include <cassert>
#include <string>

int main() {
    std::cout << "Testing Hash Table implementation..." << std::endl;
    
    // Create a Hash table with integer keys and string values
    phantomdb::storage::HashTable<int, std::string> hashTable;
    
    // Test insertion
    std::cout << "Inserting key-value pairs..." << std::endl;
    hashTable.insert(1, "one");
    hashTable.insert(2, "two");
    hashTable.insert(3, "three");
    hashTable.insert(4, "four");
    hashTable.insert(5, "five");
    
    // Test search
    std::cout << "Testing search..." << std::endl;
    std::string value;
    assert(hashTable.search(1, value) && value == "one");
    assert(hashTable.search(3, value) && value == "three");
    assert(hashTable.search(5, value) && value == "five");
    assert(!hashTable.search(10, value)); // Key doesn't exist
    
    std::cout << "Search tests passed" << std::endl;
    
    // Test updating existing keys
    std::cout << "Testing key update..." << std::endl;
    hashTable.insert(1, "updated_one");
    assert(hashTable.search(1, value) && value == "updated_one");
    
    std::cout << "Key update test passed" << std::endl;
    
    // Test removal
    std::cout << "Testing removal..." << std::endl;
    assert(hashTable.remove(3));
    assert(!hashTable.search(3, value)); // Key should no longer exist
    assert(hashTable.search(1, value) && value == "updated_one"); // Other keys should still exist
    assert(hashTable.search(5, value) && value == "five");
    
    std::cout << "Removal tests passed" << std::endl;
    
    // Test with string keys
    std::cout << "Testing with string keys..." << std::endl;
    phantomdb::storage::HashTable<std::string, int> stringHashTable;
    
    stringHashTable.insert("one", 1);
    stringHashTable.insert("two", 2);
    stringHashTable.insert("three", 3);
    
    int intValue;
    assert(stringHashTable.search("one", intValue) && intValue == 1);
    assert(stringHashTable.search("two", intValue) && intValue == 2);
    assert(stringHashTable.search("three", intValue) && intValue == 3);
    
    std::cout << "String key tests passed" << std::endl;
    
    // Test size and count
    std::cout << "Testing size and count..." << std::endl;
    assert(hashTable.getSize() == 100); // Default size
    assert(hashTable.getCount() == 4); // 5 inserted - 1 removed
    
    std::cout << "Size and count tests passed" << std::endl;
    
    std::cout << "All Hash Table tests passed!" << std::endl;
    return 0;
}