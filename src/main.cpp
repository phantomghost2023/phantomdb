#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "core/core.h"
#include "core/database.h"
#include "storage/storage_engine.h"

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "         PhantomDB Demo Application     " << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Initialize core components
    std::cout << "Initializing PhantomDB core components..." << std::endl;
    phantomdb::core::Core core;
    if (!core.initialize()) {
        std::cout << "Failed to initialize core components" << std::endl;
        return 1;
    }
    
    // Initialize storage engine
    std::cout << "Initializing storage engine..." << std::endl;
    phantomdb::storage::StorageEngine storage;
    if (!storage.initialize()) {
        std::cout << "Failed to initialize storage engine" << std::endl;
        return 1;
    }
    
    std::cout << "PhantomDB version: " << core.getVersion() << std::endl;
    std::cout << "Storage engine status: " << storage.getStatus() << std::endl;
    
    // Demonstrate database operations
    std::cout << "\n--- Database Operations Demo ---" << std::endl;
    phantomdb::core::Database db;
    
    // Create a database
    std::cout << "Creating 'testdb' database..." << std::endl;
    if (db.createDatabase("testdb")) {
        std::cout << "Successfully created database 'testdb'" << std::endl;
    } else {
        std::cout << "Failed to create database 'testdb'" << std::endl;
    }
    
    // List databases
    std::cout << "Available databases: ";
    auto databases = db.listDatabases();
    for (const auto& dbName : databases) {
        std::cout << dbName << " ";
    }
    std::cout << std::endl;
    
    // Create a table with schema
    std::cout << "Creating 'users' table in 'testdb' with schema..." << std::endl;
    std::vector<std::pair<std::string, std::string>> columns = {
        {"id", "integer"},
        {"name", "string"},
        {"email", "string"},
        {"age", "integer"}
    };
    if (db.createTable("testdb", "users", columns)) {
        std::cout << "Successfully created table 'users' with schema" << std::endl;
    } else {
        std::cout << "Failed to create table 'users'" << std::endl;
    }
    
    // Show table schema
    std::cout << "Table 'users' schema:" << std::endl;
    auto schema = db.getTableSchema("testdb", "users");
    for (const auto& column : schema) {
        std::cout << "  " << column.first << " (" << column.second << ")" << std::endl;
    }
    
    // List tables
    std::cout << "Tables in 'testdb': ";
    auto tables = db.listTables("testdb");
    for (const auto& tableName : tables) {
        std::cout << tableName << " ";
    }
    std::cout << std::endl;
    
    // Insert valid data
    std::cout << "\nInserting valid data into 'users' table..." << std::endl;
    std::unordered_map<std::string, std::string> userData1 = {
        {"id", "1"},
        {"name", "John Doe"},
        {"email", "john@example.com"},
        {"age", "30"}
    };
    if (db.insertData("testdb", "users", userData1)) {
        std::cout << "Successfully inserted user data" << std::endl;
    } else {
        std::cout << "Failed to insert user data" << std::endl;
    }
    
    std::unordered_map<std::string, std::string> userData2 = {
        {"id", "2"},
        {"name", "Jane Smith"},
        {"email", "jane@example.com"},
        {"age", "25"}
    };
    if (db.insertData("testdb", "users", userData2)) {
        std::cout << "Successfully inserted user data" << std::endl;
    } else {
        std::cout << "Failed to insert user data" << std::endl;
    }
    
    // Try to insert invalid data (should fail)
    std::cout << "\nTrying to insert invalid data (wrong type)..." << std::endl;
    std::unordered_map<std::string, std::string> invalidData = {
        {"id", "not_a_number"}, // Should be integer
        {"name", "Invalid User"},
        {"email", "invalid@example.com"},
        {"age", "30"}
    };
    if (!db.insertData("testdb", "users", invalidData)) {
        std::cout << "Correctly rejected invalid data (type validation)" << std::endl;
    } else {
        std::cout << "ERROR: Invalid data was accepted!" << std::endl;
    }
    
    // Try to insert data with unknown field (should fail)
    std::cout << "\nTrying to insert data with unknown field..." << std::endl;
    std::unordered_map<std::string, std::string> unknownFieldData = {
        {"id", "3"},
        {"name", "Unknown User"},
        {"email", "unknown@example.com"},
        {"age", "35"},
        {"unknown_field", "value"} // This field doesn't exist in schema
    };
    if (!db.insertData("testdb", "users", unknownFieldData)) {
        std::cout << "Correctly rejected data with unknown field (schema validation)" << std::endl;
    } else {
        std::cout << "ERROR: Data with unknown field was accepted!" << std::endl;
    }
    
    // Select all data
    std::cout << "\nSelecting all data from 'users' table:" << std::endl;
    auto allResults = db.selectData("testdb", "users");
    std::cout << "Found " << allResults.size() << " rows:" << std::endl;
    for (const auto& row : allResults) {
        std::cout << "  Row: ";
        for (const auto& field : row) {
            std::cout << field.first << "=" << field.second << " ";
        }
        std::cout << std::endl;
    }
    
    // Select data with condition
    std::cout << "\nSelecting data with condition (id = '1'):" << std::endl;
    auto filteredResults = db.selectData("testdb", "users", "id = '1'");
    std::cout << "Found " << filteredResults.size() << " rows:" << std::endl;
    for (const auto& row : filteredResults) {
        std::cout << "  Row: ";
        for (const auto& field : row) {
            std::cout << field.first << "=" << field.second << " ";
        }
        std::cout << std::endl;
    }
    
    // Update data
    std::cout << "\nUpdating data in 'users' table (age = 31 where id = '1')..." << std::endl;
    std::unordered_map<std::string, std::string> updateData = {
        {"age", "31"},
        {"email", "john.doe.updated@example.com"}
    };
    if (db.updateData("testdb", "users", updateData, "id = '1'")) {
        std::cout << "Successfully updated user data" << std::endl;
    } else {
        std::cout << "Failed to update user data" << std::endl;
    }
    
    // Verify update
    std::cout << "Verifying update:" << std::endl;
    auto updatedResults = db.selectData("testdb", "users", "id = '1'");
    if (!updatedResults.empty()) {
        std::cout << "  Updated row: ";
        for (const auto& field : updatedResults[0]) {
            std::cout << field.first << "=" << field.second << " ";
        }
        std::cout << std::endl;
    }
    
    // Delete data
    std::cout << "\nDeleting data from 'users' table (id = '2')..." << std::endl;
    if (db.deleteData("testdb", "users", "id = '2'")) {
        std::cout << "Successfully deleted user data" << std::endl;
    } else {
        std::cout << "Failed to delete user data" << std::endl;
    }
    
    // Verify deletion
    std::cout << "Verifying deletion (should have 1 row remaining):" << std::endl;
    auto remainingResults = db.selectData("testdb", "users");
    std::cout << "Found " << remainingResults.size() << " rows:" << std::endl;
    for (const auto& row : remainingResults) {
        std::cout << "  Row: ";
        for (const auto& field : row) {
            std::cout << field.first << "=" << field.second << " ";
        }
        std::cout << std::endl;
    }
    
    // Write to storage engine
    std::cout << "\n--- Storage Engine Demo ---" << std::endl;
    std::cout << "Writing test data to storage engine..." << std::endl;
    if (storage.writeData("Test data for PhantomDB storage engine")) {
        std::cout << "Successfully wrote data to storage engine" << std::endl;
    } else {
        std::cout << "Failed to write data to storage engine" << std::endl;
    }
    
    std::cout << "\nPhantomDB demo completed successfully!" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Shutdown components
    storage.shutdown();
    core.shutdown();
    
    return 0;
}