#include "database.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Enhanced PhantomDB Database Functionality" << std::endl;
    std::cout << "================================================" << std::endl;
    
    phantomdb::core::Database db;
    
    // Test 1: Create database
    std::cout << "\n1. Testing database creation..." << std::endl;
    assert(db.createDatabase("testdb"));
    assert(!db.createDatabase("testdb")); // Should fail - already exists
    std::cout << "✓ Database creation tests passed" << std::endl;
    
    // Test 2: Create table with schema
    std::cout << "\n2. Testing table creation with schema..." << std::endl;
    std::vector<std::pair<std::string, std::string>> columns = {
        {"id", "integer"},
        {"name", "string"},
        {"age", "integer"},
        {"email", "string"}
    };
    assert(db.createTable("testdb", "users", columns));
    assert(!db.createTable("testdb", "users", columns)); // Should fail - already exists
    std::cout << "✓ Table creation tests passed" << std::endl;
    
    // Test 3: List tables
    std::cout << "\n3. Testing table listing..." << std::endl;
    auto tables = db.listTables("testdb");
    assert(tables.size() == 1);
    assert(tables[0] == "users");
    std::cout << "✓ Table listing tests passed" << std::endl;
    
    // Test 4: Get table schema
    std::cout << "\n4. Testing schema retrieval..." << std::endl;
    auto schema = db.getTableSchema("testdb", "users");
    assert(schema.size() == 4);
    assert(schema[0].first == "id" && schema[0].second == "integer");
    assert(schema[1].first == "name" && schema[1].second == "string");
    assert(schema[2].first == "age" && schema[2].second == "integer");
    assert(schema[3].first == "email" && schema[3].second == "string");
    std::cout << "✓ Schema retrieval tests passed" << std::endl;
    
    // Test 5: Insert valid data
    std::cout << "\n5. Testing valid data insertion..." << std::endl;
    std::unordered_map<std::string, std::string> userData1 = {
        {"id", "1"},
        {"name", "John Doe"},
        {"age", "30"},
        {"email", "john@example.com"}
    };
    assert(db.insertData("testdb", "users", userData1));
    
    std::unordered_map<std::string, std::string> userData2 = {
        {"id", "2"},
        {"name", "Jane Smith"},
        {"age", "25"},
        {"email", "jane@example.com"}
    };
    assert(db.insertData("testdb", "users", userData2));
    std::cout << "✓ Valid data insertion tests passed" << std::endl;
    
    // Test 6: Insert invalid data (wrong type)
    std::cout << "\n6. Testing invalid data insertion..." << std::endl;
    std::unordered_map<std::string, std::string> invalidData = {
        {"id", "not_a_number"}, // Should be integer
        {"name", "Invalid User"},
        {"age", "30"},
        {"email", "invalid@example.com"}
    };
    // This should fail due to type validation
    assert(!db.insertData("testdb", "users", invalidData));
    std::cout << "✓ Invalid data insertion tests passed" << std::endl;
    
    // Test 7: Insert data with unknown field
    std::cout << "\n7. Testing unknown field insertion..." << std::endl;
    std::unordered_map<std::string, std::string> unknownFieldData = {
        {"id", "3"},
        {"name", "Unknown User"},
        {"age", "35"},
        {"email", "unknown@example.com"},
        {"unknown_field", "value"} // This field doesn't exist in schema
    };
    // This should fail due to unknown field
    assert(!db.insertData("testdb", "users", unknownFieldData));
    std::cout << "✓ Unknown field insertion tests passed" << std::endl;
    
    // Test 8: Select all data
    std::cout << "\n8. Testing data selection (all)..." << std::endl;
    auto allData = db.selectData("testdb", "users");
    assert(allData.size() == 2);
    std::cout << "✓ Data selection (all) tests passed" << std::endl;
    
    // Test 9: Select data with condition
    std::cout << "\n9. Testing data selection with condition..." << std::endl;
    auto filteredData = db.selectData("testdb", "users", "id = '1'");
    assert(filteredData.size() == 1);
    assert(filteredData[0].at("name") == "John Doe");
    std::cout << "✓ Data selection with condition tests passed" << std::endl;
    
    // Test 10: Select data with complex condition
    std::cout << "\n10. Testing data selection with complex condition..." << std::endl;
    auto complexFilteredData = db.selectData("testdb", "users", "age = '30' AND name = 'John Doe'");
    assert(complexFilteredData.size() == 1);
    assert(complexFilteredData[0].at("id") == "1");
    std::cout << "✓ Data selection with complex condition tests passed" << std::endl;
    
    // Test 11: Update data
    std::cout << "\n11. Testing data update..." << std::endl;
    std::unordered_map<std::string, std::string> updateData = {
        {"age", "31"},
        {"email", "john.doe.updated@example.com"}
    };
    assert(db.updateData("testdb", "users", updateData, "id = '1'"));
    
    // Verify update
    auto updatedData = db.selectData("testdb", "users", "id = '1'");
    assert(updatedData.size() == 1);
    assert(updatedData[0].at("age") == "31");
    assert(updatedData[0].at("email") == "john.doe.updated@example.com");
    std::cout << "✓ Data update tests passed" << std::endl;
    
    // Test 12: Update with invalid data
    std::cout << "\n12. Testing invalid data update..." << std::endl;
    std::unordered_map<std::string, std::string> invalidUpdateData = {
        {"age", "not_a_number"} // Should be integer
    };
    assert(!db.updateData("testdb", "users", invalidUpdateData, "id = '1'"));
    std::cout << "✓ Invalid data update tests passed" << std::endl;
    
    // Test 13: Delete data
    std::cout << "\n13. Testing data deletion..." << std::endl;
    assert(db.deleteData("testdb", "users", "id = '2'"));
    
    // Verify deletion
    auto remainingData = db.selectData("testdb", "users");
    assert(remainingData.size() == 1);
    assert(remainingData[0].at("id") == "1");
    std::cout << "✓ Data deletion tests passed" << std::endl;
    
    // Test 14: List databases
    std::cout << "\n14. Testing database listing..." << std::endl;
    auto databases = db.listDatabases();
    assert(databases.size() >= 1);
    assert(std::find(databases.begin(), databases.end(), "testdb") != databases.end());
    std::cout << "✓ Database listing tests passed" << std::endl;
    
    std::cout << "\n================================================" << std::endl;
    std::cout << "All Enhanced PhantomDB Database Tests Passed! ✓" << std::endl;
    std::cout << "================================================" << std::endl;
    
    return 0;
}