#include "query_executor.h"
#include <iostream>
#include <cassert>
#include <nlohmann/json.hpp>

int main() {
    std::cout << "Testing PhantomDB Query Executor" << std::endl;
    std::cout << "================================" << std::endl;
    
    // Initialize database
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
    
    // Test 1: Insert data
    std::cout << "\n1. Testing INSERT query..." << std::endl;
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
    std::cout << "✓ INSERT query test passed" << std::endl;
    
    // Test 2: Select all data
    std::cout << "\n2. Testing SELECT query (all)..." << std::endl;
    nlohmann::json selectAllQuery = {
        {"operation", "select"},
        {"database", "testdb"},
        {"table", "users"}
    };
    
    auto selectAllResult = executor.executeQuery(selectAllQuery);
    assert(selectAllResult["success"] == true);
    assert(selectAllResult["count"] == 1);
    assert(selectAllResult["data"][0]["name"] == "John Doe");
    std::cout << "✓ SELECT query (all) test passed" << std::endl;
    
    // Test 3: Select with conditions
    std::cout << "\n3. Testing SELECT query with conditions..." << std::endl;
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
    std::cout << "✓ SELECT query with conditions test passed" << std::endl;
    
    // Test 4: Update data
    std::cout << "\n4. Testing UPDATE query..." << std::endl;
    nlohmann::json updateQuery = {
        {"operation", "update"},
        {"database", "testdb"},
        {"table", "users"},
        {"data", {
            {"age", "31"},
            {"email", "john.doe.updated@example.com"}
        }},
        {"conditions", {
            {"id", "1"}
        }}
    };
    
    auto updateResult = executor.executeQuery(updateQuery);
    assert(updateResult["success"] == true);
    std::cout << "✓ UPDATE query test passed" << std::endl;
    
    // Test 5: Verify update
    std::cout << "\n5. Verifying UPDATE result..." << std::endl;
    auto verifyResult = executor.executeQuery(selectQuery);
    assert(verifyResult["success"] == true);
    assert(verifyResult["data"][0]["age"] == "31");
    assert(verifyResult["data"][0]["email"] == "john.doe.updated@example.com");
    std::cout << "✓ UPDATE verification test passed" << std::endl;
    
    // Test 6: Insert second record
    std::cout << "\n6. Inserting second record..." << std::endl;
    nlohmann::json insertQuery2 = {
        {"operation", "insert"},
        {"database", "testdb"},
        {"table", "users"},
        {"data", {
            {"id", "2"},
            {"name", "Jane Smith"},
            {"email", "jane@example.com"},
            {"age", "25"}
        }}
    };
    
    auto insertResult2 = executor.executeQuery(insertQuery2);
    assert(insertResult2["success"] == true);
    std::cout << "✓ Second INSERT query test passed" << std::endl;
    
    // Test 7: Select all data (should have 2 records now)
    std::cout << "\n7. Testing SELECT query (all, 2 records)..." << std::endl;
    auto selectAllResult2 = executor.executeQuery(selectAllQuery);
    assert(selectAllResult2["success"] == true);
    assert(selectAllResult2["count"] == 2);
    std::cout << "✓ SELECT query (all, 2 records) test passed" << std::endl;
    
    // Test 8: Delete data
    std::cout << "\n8. Testing DELETE query..." << std::endl;
    nlohmann::json deleteQuery = {
        {"operation", "delete"},
        {"database", "testdb"},
        {"table", "users"},
        {"conditions", {
            {"id", "2"}
        }}
    };
    
    auto deleteResult = executor.executeQuery(deleteQuery);
    assert(deleteResult["success"] == true);
    std::cout << "✓ DELETE query test passed" << std::endl;
    
    // Test 9: Verify deletion
    std::cout << "\n9. Verifying DELETE result..." << std::endl;
    auto finalResult = executor.executeQuery(selectAllQuery);
    assert(finalResult["success"] == true);
    assert(finalResult["count"] == 1);
    assert(finalResult["data"][0]["id"] == "1");
    std::cout << "✓ DELETE verification test passed" << std::endl;
    
    // Test 10: Error handling
    std::cout << "\n10. Testing error handling..." << std::endl;
    nlohmann::json invalidQuery = {
        {"operation", "invalid_operation"},
        {"database", "testdb"},
        {"table", "users"}
    };
    
    auto errorResult = executor.executeQuery(invalidQuery);
    assert(errorResult["success"] == false);
    std::cout << "✓ Error handling test passed" << std::endl;
    
    std::cout << "\n================================" << std::endl;
    std::cout << "All Query Executor Tests Passed! ✓" << std::endl;
    std::cout << "================================" << std::endl;
    
    return 0;
}