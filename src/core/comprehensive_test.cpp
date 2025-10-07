#include "database.h"
#include "utils.h"
#include "persistence.h"
#ifdef __has_include
#  if __has_include(<nlohmann/json.hpp>)
#    include <nlohmann/json.hpp>
#    define HAS_JSON 1
#    include "query_executor.h"
#  endif
#endif

#include <iostream>
#include <cassert>
#include <filesystem>

void testSchemaEnforcement() {
    std::cout << "\n=== Testing Schema Enforcement ===" << std::endl;
    
    phantomdb::core::Database db;
    db.createDatabase("testdb");
    
    // Create table with schema
    std::vector<std::pair<std::string, std::string>> columns = {
        {"id", "integer"},
        {"name", "string"},
        {"email", "string"},
        {"age", "integer"}
    };
    assert(db.createTable("testdb", "users", columns));
    
    // Test valid data insertion
    std::unordered_map<std::string, std::string> validData = {
        {"id", "1"},
        {"name", "John Doe"},
        {"email", "john@example.com"},
        {"age", "30"}
    };
    assert(db.insertData("testdb", "users", validData));
    std::cout << "✓ Valid data insertion passed" << std::endl;
    
    // Test invalid data (wrong type)
    std::unordered_map<std::string, std::string> invalidData = {
        {"id", "not_a_number"}, // Should be integer
        {"name", "Invalid User"},
        {"email", "invalid@example.com"},
        {"age", "30"}
    };
    assert(!db.insertData("testdb", "users", invalidData));
    std::cout << "✓ Invalid data (wrong type) rejection passed" << std::endl;
    
    // Test unknown field
    std::unordered_map<std::string, std::string> unknownFieldData = {
        {"id", "2"},
        {"name", "Unknown User"},
        {"email", "unknown@example.com"},
        {"age", "35"},
        {"unknown_field", "value"} // This field doesn't exist in schema
    };
    assert(!db.insertData("testdb", "users", unknownFieldData));
    std::cout << "✓ Unknown field rejection passed" << std::endl;
}

void testConditionProcessing() {
    std::cout << "\n=== Testing Condition Processing ===" << std::endl;
    
    phantomdb::core::Database db;
    db.createDatabase("testdb");
    
    // Create table with schema
    std::vector<std::pair<std::string, std::string>> columns = {
        {"id", "integer"},
        {"name", "string"},
        {"email", "string"},
        {"age", "integer"}
    };
    assert(db.createTable("testdb", "users", columns));
    
    // Insert test data
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
    
    std::unordered_map<std::string, std::string> userData3 = {
        {"id", "3"},
        {"name", "Bob Johnson"},
        {"email", "bob@example.com"},
        {"age", "30"}
    };
    assert(db.insertData("testdb", "users", userData3));
    
    // Test condition parsing utility
    auto condition1 = phantomdb::core::utils::parseCondition("id = '1'");
    assert(condition1.size() == 1);
    assert(condition1["id"] == "1");
    std::cout << "✓ Simple condition parsing passed" << std::endl;
    
    auto condition2 = phantomdb::core::utils::parseCondition("age = '30' AND name = 'John Doe'");
    assert(condition2.size() == 2);
    assert(condition2["age"] == "30");
    assert(condition2["name"] == "John Doe");
    std::cout << "✓ Complex condition parsing passed" << std::endl;
    
    // Test select with conditions
    auto results1 = db.selectData("testdb", "users", "id = '1'");
    assert(results1.size() == 1);
    assert(results1[0].at("name") == "John Doe");
    std::cout << "✓ Select with simple condition passed" << std::endl;
    
    auto results2 = db.selectData("testdb", "users", "age = '30'");
    assert(results2.size() == 2);
    std::cout << "✓ Select with value condition passed" << std::endl;
    
    auto results3 = db.selectData("testdb", "users", "age = '30' AND name = 'John Doe'");
    assert(results3.size() == 1);
    assert(results3[0].at("id") == "1");
    std::cout << "✓ Select with complex condition passed" << std::endl;
    
    // Test update with conditions
    std::unordered_map<std::string, std::string> updateData = {
        {"email", "john.doe.updated@example.com"}
    };
    assert(db.updateData("testdb", "users", updateData, "id = '1'"));
    
    auto updatedResults = db.selectData("testdb", "users", "id = '1'");
    assert(updatedResults.size() == 1);
    assert(updatedResults[0].at("email") == "john.doe.updated@example.com");
    std::cout << "✓ Update with condition passed" << std::endl;
    
    // Test delete with conditions
    assert(db.deleteData("testdb", "users", "id = '2'"));
    
    auto remainingResults = db.selectData("testdb", "users");
    assert(remainingResults.size() == 2);
    std::cout << "✓ Delete with condition passed" << std::endl;
}

void testPersistence() {
    std::cout << "\n=== Testing Persistence ===" << std::endl;
    
    // Clean up any existing test data
    std::filesystem::remove_all("./data/testdb.db");
    
    // Create and populate database
    phantomdb::core::Database db;
    db.createDatabase("testdb");
    
    std::vector<std::pair<std::string, std::string>> columns = {
        {"id", "integer"},
        {"name", "string"},
        {"email", "string"},
        {"age", "integer"}
    };
    assert(db.createTable("testdb", "users", columns));
    
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
    std::cout << "✓ Database saving passed" << std::endl;
    
    // Load database from disk
    phantomdb::core::Database db2;
    assert(db2.loadFromDisk("testdb"));
    
    // Verify data
    auto tables = db2.listTables("testdb");
    assert(tables.size() == 1);
    assert(tables[0] == "users");
    
    auto schema = db2.getTableSchema("testdb", "users");
    assert(schema.size() == 4);
    
    auto data = db2.selectData("testdb", "users");
    assert(data.size() == 2);
    
    bool foundFirst = false;
    bool foundSecond = false;
    
    for (const auto& row : data) {
        if (row.at("id") == "1" && row.at("name") == "John Doe") {
            foundFirst = true;
        } else if (row.at("id") == "2" && row.at("name") == "Jane Smith") {
            foundSecond = true;
        }
    }
    
    assert(foundFirst);
    assert(foundSecond);
    std::cout << "✓ Database loading passed" << std::endl;
    
    // Clean up test data
    std::filesystem::remove_all("./data");
}

#ifdef HAS_JSON
void testQueryExecutor() {
    std::cout << "\n=== Testing Query Executor ===" << std::endl;
    
    phantomdb::core::Database db;
    db.createDatabase("testdb");
    
    // Create table with schema
    std::vector<std::pair<std::string, std::string>> columns = {
        {"id", "integer"},
        {"name", "string"},
        {"email", "string"},
        {"age", "integer"}
    };
    db.createTable("testdb", "users", columns);
    
    // Initialize query executor
    phantomdb::core::QueryExecutor executor(db);
    
    // Test INSERT query
    nlohmann::json insertQuery = {
        {"operation", "insert"},
        {"database", "testdb"},
        {"table", "users"},
        {"data", {
            {"id", "1"},
            {"name", "John Doe"},
            {"email", "john@example.com"},
            {"age", "30"}
        }}
    };
    
    auto insertResult = executor.executeQuery(insertQuery);
    assert(insertResult["success"] == true);
    std::cout << "✓ INSERT query execution passed" << std::endl;
    
    // Test SELECT query
    nlohmann::json selectQuery = {
        {"operation", "select"},
        {"database", "testdb"},
        {"table", "users"},
        {"conditions", {
            {"id", "1"}
        }}
    };
    
    auto selectResult = executor.executeQuery(selectQuery);
    assert(selectResult["success"] == true);
    assert(selectResult["count"] == 1);
    assert(selectResult["data"][0]["name"] == "John Doe");
    std::cout << "✓ SELECT query execution passed" << std::endl;
    
    // Test UPDATE query
    nlohmann::json updateQuery = {
        {"operation", "update"},
        {"database", "testdb"},
        {"table", "users"},
        {"data", {
            {"age", "31"}
        }},
        {"conditions", {
            {"id", "1"}
        }}
    };
    
    auto updateResult = executor.executeQuery(updateQuery);
    assert(updateResult["success"] == true);
    std::cout << "✓ UPDATE query execution passed" << std::endl;
    
    // Test DELETE query
    nlohmann::json deleteQuery = {
        {"operation", "delete"},
        {"database", "testdb"},
        {"table", "users"},
        {"conditions", {
            {"id", "1"}
        }}
    };
    
    auto deleteResult = executor.executeQuery(deleteQuery);
    assert(deleteResult["success"] == true);
    std::cout << "✓ DELETE query execution passed" << std::endl;
}
#endif

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  PhantomDB Comprehensive Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        testSchemaEnforcement();
        testConditionProcessing();
        testPersistence();
        
#ifdef HAS_JSON
        testQueryExecutor();
#endif
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "  All Comprehensive Tests Passed! ✓" << std::endl;
        std::cout << "========================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}