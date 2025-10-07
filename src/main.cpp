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
    
    // Create a table
    std::cout << "Creating 'users' table in 'testdb'..." << std::endl;
    std::vector<std::pair<std::string, std::string>> columns = {
        {"id", "INT"},
        {"name", "VARCHAR(50)"},
        {"email", "VARCHAR(100)"}
    };
    if (db.createTable("testdb", "users", columns)) {
        std::cout << "Successfully created table 'users'" << std::endl;
    } else {
        std::cout << "Failed to create table 'users'" << std::endl;
    }
    
    // List tables
    std::cout << "Tables in 'testdb': ";
    auto tables = db.listTables("testdb");
    for (const auto& tableName : tables) {
        std::cout << tableName << " ";
    }
    std::cout << std::endl;
    
    // Insert data
    std::cout << "Inserting data into 'users' table..." << std::endl;
    std::unordered_map<std::string, std::string> userData1 = {
        {"id", "1"},
        {"name", "John Doe"},
        {"email", "john@example.com"}
    };
    if (db.insertData("testdb", "users", userData1)) {
        std::cout << "Successfully inserted user data" << std::endl;
    } else {
        std::cout << "Failed to insert user data" << std::endl;
    }
    
    std::unordered_map<std::string, std::string> userData2 = {
        {"id", "2"},
        {"name", "Jane Smith"},
        {"email", "jane@example.com"}
    };
    if (db.insertData("testdb", "users", userData2)) {
        std::cout << "Successfully inserted user data" << std::endl;
    } else {
        std::cout << "Failed to insert user data" << std::endl;
    }
    
    // Select data
    std::cout << "Selecting data from 'users' table:" << std::endl;
    auto results = db.selectData("testdb", "users");
    for (const auto& row : results) {
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