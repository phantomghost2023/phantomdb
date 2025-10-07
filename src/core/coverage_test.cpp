#include "database.h"
#include "utils.h"
#include <iostream>
#include <cassert>

// Simple test to demonstrate coverage reporting
int main() {
    std::cout << "Running coverage test..." << std::endl;
    
    // Test database creation
    phantomdb::core::Database db;
    bool created = db.createDatabase("test_coverage_db");
    assert(created);
    
    // Test table creation
    std::vector<std::pair<std::string, std::string>> columns = {
        {"id", "integer"},
        {"name", "string"}
    };
    bool tableCreated = db.createTable("test_coverage_db", "test_table", columns);
    assert(tableCreated);
    
    // Test data insertion
    std::unordered_map<std::string, std::string> data = {
        {"id", "1"},
        {"name", "Test User"}
    };
    bool inserted = db.insertData("test_coverage_db", "test_table", data);
    assert(inserted);
    
    // Test data selection
    auto results = db.selectData("test_coverage_db", "test_table");
    assert(!results.empty());
    
    // Test utility functions
    bool isValid = phantomdb::core::isValidInteger("123");
    assert(isValid);
    
    isValid = phantomdb::core::isValidInteger("abc");
    assert(!isValid);
    
    std::cout << "Coverage test completed successfully!" << std::endl;
    return 0;
}