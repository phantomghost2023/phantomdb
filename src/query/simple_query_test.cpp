#include "query_processor.h"
#include <iostream>

using namespace phantomdb::query;

int main() {
    std::cout << "Testing QueryProcessor compilation..." << std::endl;
    
    // Create a QueryProcessor instance
    QueryProcessor processor;
    
    // Initialize the processor
    if (!processor.initialize()) {
        std::cout << "Failed to initialize QueryProcessor!" << std::endl;
        return 1;
    }
    
    // Test a simple query
    std::vector<std::vector<std::string>> results;
    std::string errorMsg;
    
    bool success = processor.executeQuery("SELECT * FROM test_table", results, errorMsg);
    
    if (success) {
        std::cout << "QueryProcessor compilation test passed!" << std::endl;
        return 0;
    } else {
        std::cout << "QueryProcessor compilation test failed: " << errorMsg << std::endl;
        return 1;
    }
}