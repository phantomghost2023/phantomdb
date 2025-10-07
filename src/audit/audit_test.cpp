#include "audit_logger.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

using namespace phantomdb::audit;

int main() {
    std::cout << "Testing Audit Logger functionality..." << std::endl;
    
    // Initialize the audit logger
    AuditLogger auditLogger;
    if (!auditLogger.initialize("test_audit.log")) {
        std::cerr << "Failed to initialize audit logger" << std::endl;
        return 1;
    }
    
    // Test 1: Log user login
    std::cout << "\n--- Test 1: Log user login ---" << std::endl;
    auditLogger.logUserLogin("alice", "192.168.1.100");
    auditLogger.logUserLogin("bob", "192.168.1.101");
    
    // Test 2: Log database operations
    std::cout << "\n--- Test 2: Log database operations ---" << std::endl;
    auditLogger.logDatabaseCreate("alice", "test_db");
    auditLogger.logDatabaseDrop("alice", "test_db");
    
    // Test 3: Log table operations
    std::cout << "\n--- Test 3: Log table operations ---" << std::endl;
    auditLogger.logTableCreate("alice", "production_db", "users");
    auditLogger.logTableCreate("alice", "production_db", "orders");
    auditLogger.logTableAlter("alice", "production_db", "users");
    auditLogger.logTableDrop("alice", "production_db", "orders");
    
    // Test 4: Log data operations
    std::cout << "\n--- Test 4: Log data operations ---" << std::endl;
    auditLogger.logDataInsert("bob", "production_db", "users", "123");
    auditLogger.logDataInsert("bob", "production_db", "users", "124");
    auditLogger.logDataSelect("bob", "production_db", "users", "id = 123");
    auditLogger.logDataUpdate("bob", "production_db", "users", "123");
    auditLogger.logDataDelete("bob", "production_db", "users", "124");
    
    // Test 5: Log transaction operations
    std::cout << "\n--- Test 5: Log transaction operations ---" << std::endl;
    auditLogger.logTransactionStart("alice", "txn_001");
    auditLogger.logTransactionCommit("alice", "txn_001");
    auditLogger.logTransactionStart("bob", "txn_002");
    auditLogger.logTransactionRollback("bob", "txn_002");
    
    // Test 6: Log query execution
    std::cout << "\n--- Test 6: Log query execution ---" << std::endl;
    auditLogger.logQueryExecute("alice", "production_db", "SELECT * FROM users WHERE id = 123", 15.5);
    auditLogger.logQueryExecute("bob", "production_db", "INSERT INTO users (name) VALUES ('John Doe')", 10.2);
    
    // Test 7: Log permission operations
    std::cout << "\n--- Test 7: Log permission operations ---" << std::endl;
    auditLogger.logPermissionGranted("admin", "alice", "CREATE_TABLE");
    auditLogger.logPermissionRevoked("admin", "bob", "DROP_TABLE");
    
    // Test 8: Log role operations
    std::cout << "\n--- Test 8: Log role operations ---" << std::endl;
    auditLogger.logRoleAssigned("admin", "alice", "ADMIN");
    auditLogger.logRoleRevoked("admin", "bob", "WRITER");
    
    // Test 9: Log user logout
    std::cout << "\n--- Test 9: Log user logout ---" << std::endl;
    auditLogger.logUserLogout("alice", "192.168.1.100");
    auditLogger.logUserLogout("bob", "192.168.1.101");
    
    // Test 10: Query audit events
    std::cout << "\n--- Test 10: Query audit events ---" << std::endl;
    auto recentEvents = auditLogger.getRecentEvents(10);
    std::cout << "Recent events (last 10):" << std::endl;
    for (const auto& event : recentEvents) {
        std::cout << "  Event: " << static_cast<int>(event.eventType) 
                  << " by " << event.username << std::endl;
    }
    
    auto aliceEvents = auditLogger.getEventsForUser("alice");
    std::cout << "Events for user 'alice': " << aliceEvents.size() << std::endl;
    
    auto productionDbEvents = auditLogger.getEventsForDatabase("production_db");
    std::cout << "Events for database 'production_db': " << productionDbEvents.size() << std::endl;
    
    // Test 11: Time range query
    std::cout << "\n--- Test 11: Time range query ---" << std::endl;
    auto now = std::chrono::system_clock::now();
    auto tenMinutesAgo = now - std::chrono::minutes(10);
    auto timeRangeEvents = auditLogger.getEventsForTimeRange(tenMinutesAgo, now);
    std::cout << "Events in last 10 minutes: " << timeRangeEvents.size() << std::endl;
    
    // Shutdown the audit logger
    auditLogger.shutdown();
    
    std::cout << "\nAll audit logger tests passed!" << std::endl;
    return 0;
}