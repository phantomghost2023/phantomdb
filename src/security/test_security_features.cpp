#include "rbac.h"
#include "audit_logger.h"
#include <iostream>
#include <cassert>
#include <chrono>
#include <thread>

using namespace phantomdb::security;
using namespace phantomdb::audit;

int main() {
    std::cout << "Testing PhantomDB Security Features..." << std::endl;
    
    // Test RBAC functionality
    std::cout << "Testing RBAC..." << std::endl;
    
    RBACManager rbac;
    assert(rbac.initialize() == true);
    
    // Test user creation
    assert(rbac.createUser("admin", "admin123") == true);
    assert(rbac.createUser("writer", "writer123") == true);
    assert(rbac.createUser("reader", "reader123") == true);
    
    std::cout << "User creation test passed" << std::endl;
    
    // Test user authentication
    assert(rbac.authenticateUser("admin", "admin123") == true);
    assert(rbac.authenticateUser("writer", "writer123") == true);
    assert(rbac.authenticateUser("reader", "reader123") == true);
    assert(rbac.authenticateUser("admin", "wrong_password") == false);
    
    std::cout << "User authentication test passed" << std::endl;
    
    // Test role assignment
    assert(rbac.assignRole("admin", UserRole::ADMIN) == true);
    assert(rbac.assignRole("writer", UserRole::WRITER) == true);
    assert(rbac.assignRole("reader", UserRole::READER) == true);
    
    std::cout << "Role assignment test passed" << std::endl;
    
    // Test role retrieval
    assert(rbac.getUserRole("admin") == UserRole::ADMIN);
    assert(rbac.getUserRole("writer") == UserRole::WRITER);
    assert(rbac.getUserRole("reader") == UserRole::READER);
    
    std::cout << "Role retrieval test passed" << std::endl;
    
    // Test permission checking
    assert(rbac.hasPermission("admin", Permission::CREATE_DATABASE) == true);
    assert(rbac.hasPermission("admin", Permission::DROP_DATABASE) == true);
    assert(rbac.hasPermission("admin", Permission::INSERT) == true);
    assert(rbac.hasPermission("admin", Permission::SELECT) == true);
    
    assert(rbac.hasPermission("writer", Permission::INSERT) == true);
    assert(rbac.hasPermission("writer", Permission::SELECT) == true);
    assert(rbac.hasPermission("writer", Permission::CREATE_DATABASE) == false);
    
    assert(rbac.hasPermission("reader", Permission::SELECT) == true);
    assert(rbac.hasPermission("reader", Permission::INSERT) == false);
    assert(rbac.hasPermission("reader", Permission::CREATE_DATABASE) == false);
    
    std::cout << "Permission checking test passed" << std::endl;
    
    // Test user listing
    auto users = rbac.listUsers();
    assert(users.size() >= 3);
    assert(std::find(users.begin(), users.end(), "admin") != users.end());
    assert(std::find(users.begin(), users.end(), "writer") != users.end());
    assert(std::find(users.begin(), users.end(), "reader") != users.end());
    
    std::cout << "User listing test passed" << std::endl;
    
    // Test permission listing
    auto adminPermissions = rbac.listUserPermissions("admin");
    assert(!adminPermissions.empty());
    
    auto writerPermissions = rbac.listUserPermissions("writer");
    assert(!writerPermissions.empty());
    assert(writerPermissions.size() < adminPermissions.size());
    
    auto readerPermissions = rbac.listUserPermissions("reader");
    assert(!readerPermissions.empty());
    assert(readerPermissions.size() < writerPermissions.size());
    
    std::cout << "Permission listing test passed" << std::endl;
    
    // Test Audit Logging functionality
    std::cout << "Testing Audit Logging..." << std::endl;
    
    AuditLogger auditLogger;
    assert(auditLogger.initialize("test_audit.log") == true);
    
    // Test logging various events
    auditLogger.logUserLogin("admin", "192.168.1.100");
    auditLogger.logDatabaseCreate("admin", "testdb");
    auditLogger.logTableCreate("admin", "testdb", "users");
    auditLogger.logDataInsert("admin", "testdb", "users", "user_123");
    auditLogger.logQueryExecute("admin", "testdb", "SELECT * FROM users", 1.5);
    auditLogger.logTransactionStart("admin", "txn_456");
    auditLogger.logTransactionCommit("admin", "txn_456");
    auditLogger.logUserLogout("admin", "192.168.1.100");
    
    std::cout << "Event logging test passed" << std::endl;
    
    // Test retrieving events
    auto recentEvents = auditLogger.getRecentEvents(10);
    assert(recentEvents.size() >= 8);
    
    auto userEvents = auditLogger.getEventsForUser("admin");
    assert(userEvents.size() >= 8);
    
    // Test time range queries
    auto now = std::chrono::system_clock::now();
    auto tenMinutesAgo = now - std::chrono::minutes(10);
    
    auto timeRangeEvents = auditLogger.getEventsForTimeRange(tenMinutesAgo, now);
    assert(!timeRangeEvents.empty());
    
    std::cout << "Event retrieval test passed" << std::endl;
    
    // Test RBAC and Audit Logging integration
    std::cout << "Testing RBAC and Audit Logging Integration..." << std::endl;
    
    // Simulate a permission check with logging
    if (rbac.hasPermission("reader", Permission::INSERT)) {
        std::cout << "Reader has INSERT permission" << std::endl;
    } else {
        std::cout << "Reader does not have INSERT permission" << std::endl;
    }
    
    // Simulate a database operation with security checking
    std::string currentUser = "reader";
    if (rbac.hasPermission(currentUser, Permission::SELECT)) {
        auditLogger.logDataSelect(currentUser, "testdb", "users");
        std::cout << "Reader performed SELECT operation" << std::endl;
    } else {
        std::cout << "Reader denied SELECT operation" << std::endl;
    }
    
    std::cout << "Integration test passed" << std::endl;
    
    // Cleanup
    rbac.shutdown();
    auditLogger.shutdown();
    
    std::cout << "All security feature tests passed!" << std::endl;
    
    return 0;
}