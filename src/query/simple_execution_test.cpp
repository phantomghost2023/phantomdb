#include "query_processor.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;

int main() {
    std::cout << "Testing Query Processor with Execution Engine..." << std::endl;
    
    // Create query processor
    QueryProcessor processor;
    assert(processor.initialize());
    
    // Test a simple query
    std::vector<std::vector<std::string>> results;
    std::string errorMsg;
    
    bool success = processor.executeQuery("SELECT * FROM users", results, errorMsg);
    
    if (success) {
        std::cout << "Query executed successfully!" << std::endl;
        std::cout << "Results:" << std::endl;
        for (const auto& row : results) {
            for (const auto& cell : row) {
                std::cout << cell << " ";
            }
            std::cout << std::endl;
        }
    } else {
        std::cout << "Query execution failed: " << errorMsg << std::endl;
    }
    
    processor.shutdown();
    
    return 0;
}