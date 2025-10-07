#include "database.h"
#include <iostream>
#include <cassert>
#include <filesystem>

int main() {
    std::cout << "Testing PhantomDB Persistence" << std::endl;
    std::cout << "============================" << std::endl;
    
    // Clean up any existing test data
    std::filesystem::remove_all("./data/testdb.db");
    
    // Test 1: Create database and save to disk
    std::cout << "\n1. Testing database creation and saving..." << std::endl;
    phantomdb::core::Database db;
    
    // Create database
    assert(db.createDatabase("testdb"));
    
    // Create table with schema
    std::vector<std::pair<std::string, std::string>> columns = {
        {"id", "integer"},
        {"name", "string"},
        {"email", "string"},
        {"age", "integer"}
    };
    assert(db.createTable("testdb", "users", columns));
    
    // Insert data
    std::unordered_map<std::string, std::string> userData1 = {
        {"id", "1"},
        {"name", "John Doe"},
        {"email", "john@example.com"},
        {"age", "30"}
    };
    assert(db.insertData("testdb", "users", userData1));
    
    std::unordered_map<std::string, std::string> userData2 = {
        {"id", "2"},
        {"name", "Jane Smith"},
        {"email", "jane@example.com"},
        {"age", "25"}
    };
    assert(db.insertData("testdb", "users", userData2));
    
    // Save to disk
    assert(db.saveToDisk("testdb"));
    std::cout << "✓ Database creation and saving test passed" << std::endl;
    
    // Test 2: Load database from disk
    std::cout << "\n2. Testing database loading..." << std::endl;
    phantomdb::core::Database db2;
    
    // Load from disk
    assert(db2.loadFromDisk("testdb"));
    
    // Verify data
    auto tables = db2.listTables("testdb");
    assert(tables.size() == 1);
    assert(tables[0] == "users");
    
    auto schema = db2.getTableSchema("testdb", "users");
    assert(schema.size() == 4);
    
    auto data = db2.selectData("testdb", "users");
    assert(data.size() == 2);
    
    // Verify first row
    bool foundFirst = false;
    bool foundSecond = false;
    
    for (const auto& row : data) {
        if (row.at("id") == "1" && row.at("name") == "John Doe") {
            foundFirst = true;
            assert(row.at("email") == "john@example.com");
            assert(row.at("age") == "30");
        } else if (row.at("id") == "2" && row.at("name") == "Jane Smith") {
            foundSecond = true;
            assert(row.at("email") == "jane@example.com");
            assert(row.at("age") == "25");
        }
    }
    
    assert(foundFirst);
    assert(foundSecond);
    std::cout << "✓ Database loading test passed" << std::endl;
    
    // Test 3: Modify loaded database and save again
    std::cout << "\n3. Testing database modification and re-saving..." << std::endl;
    
    // Update data
    std::unordered_map<std::string, std::string> updateData = {
        {"age", "31"},
        {"email", "john.doe.updated@example.com"}
    };
    assert(db2.updateData("testdb", "users", updateData, "id = '1'"));
    
    // Insert new data
    std::unordered_map<std::string, std::string> newUser = {
        {"id", "3"},
        {"name", "Bob Johnson"},
        {"email", "bob@example.com"},
        {"age", "35"}
    };
    assert(db2.insertData("testdb", "users", newUser));
    
    // Save to different file
    assert(db2.saveToDisk("testdb", "testdb_modified.db"));
    std::cout << "✓ Database modification and re-saving test passed" << std::endl;
    
    // Test 4: Load modified database
    std::cout << "\n4. Testing modified database loading..." << std::endl;
    phantomdb::core::Database db3;
    
    // Load from modified file
    assert(db3.loadFromDisk("testdb", "testdb_modified.db"));
    
    // Verify modified data
    auto modifiedData = db3.selectData("testdb", "users");
    assert(modifiedData.size() == 3);
    
    // Check updated row
    bool foundUpdated = false;
    bool foundNew = false;
    
    for (const auto& row : modifiedData) {
        if (row.at("id") == "1") {
            assert(row.at("age") == "31");
            assert(row.at("email") == "john.doe.updated@example.com");
            foundUpdated = true;
        } else if (row.at("id") == "3") {
            assert(row.at("name") == "Bob Johnson");
            foundNew = true;
        }
    }
    
    assert(foundUpdated);
    assert(foundNew);
    std::cout << "✓ Modified database loading test passed" << std::endl;
    
    // Clean up test data
    std::filesystem::remove_all("./data");
    
    std::cout << "\n============================" << std::endl;
    std::cout << "All Persistence Tests Passed! ✓" << std::endl;
    std::cout << "============================" << std::endl;
    
    return 0;
}