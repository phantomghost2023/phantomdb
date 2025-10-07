#include "audit_logger.h"
#include <iostream>
#include <memory>

using namespace phantomdb::audit;

// Mock database class to demonstrate audit logging integration
class MockDatabase {
public:
    MockDatabase(std::shared_ptr<AuditLogger> auditLogger) : auditLogger_(auditLogger) {}
    
    bool createTable(const std::string& username, const std::string& databaseName, const std::string& tableName) {
        std::cout << "Creating table " << tableName << " in database " << databaseName 
                  << " by user " << username << std::endl;
        
        // Log the operation
        auditLogger_->logTableCreate(username, databaseName, tableName);
        
        return true;
    }
    
    bool insertData(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& recordId) {
        std::cout << "Inserting record " << recordId << " into table " << tableName 
                  << " in database " << databaseName << " by user " << username << std::endl;
        
        // Log the operation
        auditLogger_->logDataInsert(username, databaseName, tableName, recordId);
        
        return true;
    }
    
    bool selectData(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& condition) {
        std::cout << "Selecting data from table " << tableName 
                  << " in database " << databaseName << " by user " << username
                  << " with condition: " << condition << std::endl;
        
        // Log the operation
        auditLogger_->logDataSelect(username, databaseName, tableName, condition);
        
        return true;
    }
    
    bool updateData(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& recordId) {
        std::cout << "Updating record " << recordId << " in table " << tableName 
                  << " in database " << databaseName << " by user " << username << std::endl;
        
        // Log the operation
        auditLogger_->logDataUpdate(username, databaseName, tableName, recordId);
        
        return true;
    }
    
    bool deleteData(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& recordId) {
        std::cout << "Deleting record " << recordId << " from table " << tableName 
                  << " in database " << databaseName << " by user " << username << std::endl;
        
        // Log the operation
        auditLogger_->logDataDelete(username, databaseName, tableName, recordId);
        
        return true;
    }

private:
    std::shared_ptr<AuditLogger> auditLogger_;
};

int main() {
    std::cout << "Testing audit logging integration with database operations..." << std::endl;
    
    // Initialize the audit logger
    auto auditLogger = std::make_shared<AuditLogger>();
    if (!auditLogger->initialize("db_audit_test.log")) {
        std::cerr << "Failed to initialize audit logger" << std::endl;
        return 1;
    }
    
    // Log user login
    auditLogger->logUserLogin("db_admin", "192.168.1.100");
    
    // Create mock database
    MockDatabase db(auditLogger);
    
    // Perform database operations
    std::cout << "\n--- Performing database operations ---" << std::endl;
    db.createTable("db_admin", "production", "users");
    db.insertData("db_admin", "production", "users", "1001");
    db.insertData("db_admin", "production", "users", "1002");
    db.selectData("db_admin", "production", "users", "id > 1000");
    db.updateData("db_admin", "production", "users", "1001");
    db.deleteData("db_admin", "production", "users", "1002");
    
    // Log user logout
    auditLogger->logUserLogout("db_admin", "192.168.1.100");
    
    // Query audit events
    std::cout << "\n--- Audit Events ---" << std::endl;
    auto recentEvents = auditLogger->getRecentEvents(20);
    std::cout << "Total events logged: " << recentEvents.size() << std::endl;
    
    for (const auto& event : recentEvents) {
        std::cout << "Event: " << static_cast<int>(event.eventType) 
                  << " by " << event.username 
                  << " on " << event.database << "." << event.table
                  << std::endl;
    }
    
    // Shutdown the audit logger
    auditLogger->shutdown();
    
    std::cout << "\nAudit logging integration test completed!" << std::endl;
    return 0;
}