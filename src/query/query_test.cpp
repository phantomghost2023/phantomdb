#include "query_processor.h"
#include <iostream>
#include <cassert>

using namespace phantomdb::query;

void testQueryProcessorCreation() {
    std::cout << "Testing QueryProcessor creation..." << std::endl;
    
    QueryProcessor processor;
    assert(processor.initialize());
    
    std::cout << "QueryProcessor creation test passed!" << std::endl;
}

void testParseQuery() {
    std::cout << "Testing query parsing..." << std::endl;
    
    QueryProcessor processor;
    processor.initialize();
    
    std::string errorMsg;
    bool result = processor.parseQuery("SELECT * FROM users", errorMsg);
    assert(result);
    
    std::cout << "Query parsing test passed!" << std::endl;
}

void testPlanQuery() {
    std::cout << "Testing query planning..." << std::endl;
    
    QueryProcessor processor;
    processor.initialize();
    
    std::string plan;
    std::string errorMsg;
    bool result = processor.planQuery("SELECT * FROM users", plan, errorMsg);
    assert(result);
    assert(!plan.empty());
    
    std::cout << "Query planning test passed!" << std::endl;
}

void testExecuteQuery() {
    std::cout << "Testing query execution..." << std::endl;
    
    QueryProcessor processor;
    processor.initialize();
    
    std::vector<std::vector<std::string>> results;
    std::string errorMsg;
    bool result = processor.executeQuery("SELECT * FROM users", results, errorMsg);
    assert(result);
    assert(!results.empty());
    
    // Check that we have at least the header row
    assert(results.size() >= 1);
    
    std::cout << "Query execution test passed!" << std::endl;
}

int main() {
    std::cout << "Running QueryProcessor tests..." << std::endl;
    
    testQueryProcessorCreation();
    testParseQuery();
    testPlanQuery();
    testExecuteQuery();
    
    std::cout << "All QueryProcessor tests passed!" << std::endl;
    return 0;
}