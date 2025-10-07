#include "database.h"
#include "enhanced_persistence.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Enhanced Persistence Manager..." << std::endl;
    
    // Create a database instance
    phantomdb::core::Database db;
    
    // Set data directory
    db.setDataDirectory("./test_data");
    
    // Enable snapshots
    db.setSnapshotEnabled(true);
    db.setSnapshotInterval(5); // Create snapshot every 5 operations
    
    // Create a test database
    assert(db.createDatabase("test_db"));
    
    // Create a test table
    std::vector<std::pair<std::string, std::string>> columns = {
        {"id", "int"},
        {"name", "string"},
        {"email", "string"}
    };
    assert(db.createTable("test_db", "users", columns));
    
    // Insert some test data
    std::unordered_map<std::string, std::string> user1 = {
        {"id", "1"},
        {"name", "John Doe"},
        {"email", "john@example.com"}
    };
    assert(db.insertData("test_db", "users", user1));
    
    std::unordered_map<std::string, std::string> user2 = {
        {"id", "2"},
        {"name", "Jane Smith"},
        {"email", "jane@example.com"}
    };
    assert(db.insertData("test_db", "users", user2));
    
    // Save to disk
    assert(db.saveToDisk("test_db"));
    
    // Load from disk
    phantomdb::core::Database db2;
    db2.setDataDirectory("./test_data");
    assert(db2.loadFromDisk("test_db"));
    
    // Verify loaded data
    auto users = db2.selectData("test_db", "users");
    assert(users.size() == 2);
    
    // Test transaction logging
    assert(db.appendTransactionLog("test_db", "TEST_OPERATION", {{"key", "value"}}));
    
    // Test manual snapshot
    assert(db.createSnapshot("test_db"));
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}