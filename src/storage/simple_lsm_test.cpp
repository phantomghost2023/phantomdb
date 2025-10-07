#include "lsm_tree.h"
#include <iostream>
#include <string>

using namespace phantomdb::storage;

int main() {
    std::cout << "Testing LSM-tree compilation..." << std::endl;
    
    // Create an LSM-tree instance
    LSMTREE<int, std::string> lsmTree;
    
    // Test basic operations
    lsmTree.insert(1, "test_value");
    
    std::string value;
    bool found = lsmTree.search(1, value);
    
    if (found && value == "test_value") {
        std::cout << "LSM-tree compilation test passed!" << std::endl;
        return 0;
    } else {
        std::cout << "LSM-tree compilation test failed!" << std::endl;
        return 1;
    }
}