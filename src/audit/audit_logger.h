#ifndef PHANTOMDB_AUDIT_LOGGER_H
#define PHANTOMDB_AUDIT_LOGGER_H

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <unordered_map>

namespace phantomdb {
namespace audit {

// Audit event types
enum class AuditEventType {
    USER_LOGIN,
    USER_LOGOUT,
    DATABASE_CREATE,
    DATABASE_DROP,
    TABLE_CREATE,
    TABLE_DROP,
    TABLE_ALTER,
    INDEX_CREATE,
    INDEX_DROP,
    DATA_INSERT,
    DATA_SELECT,
    DATA_UPDATE,
    DATA_DELETE,
    TRANSACTION_START,
    TRANSACTION_COMMIT,
    TRANSACTION_ROLLBACK,
    QUERY_EXECUTE,
    PERMISSION_GRANTED,
    PERMISSION_REVOKED,
    ROLE_ASSIGNED,
    ROLE_REVOKED
};

// Audit event structure
struct AuditEvent {
    std::chrono::system_clock::time_point timestamp;
    std::string username;
    std::string database;
    std::string table;
    AuditEventType eventType;
    std::string details;
    std::string ipAddress;
};

class AuditLogger {
public:
    AuditLogger();
    ~AuditLogger();
    
    // Initialize the audit logger
    bool initialize(const std::string& logFilePath = "audit.log");
    
    // Shutdown the audit logger
    void shutdown();
    
    // Log an audit event
    void logEvent(const AuditEvent& event);
    
    // Log a user login
    void logUserLogin(const std::string& username, const std::string& ipAddress = "");
    
    // Log a user logout
    void logUserLogout(const std::string& username, const std::string& ipAddress = "");
    
    // Log database operations
    void logDatabaseCreate(const std::string& username, const std::string& databaseName);
    void logDatabaseDrop(const std::string& username, const std::string& databaseName);
    
    // Log table operations
    void logTableCreate(const std::string& username, const std::string& databaseName, const std::string& tableName);
    void logTableDrop(const std::string& username, const std::string& databaseName, const std::string& tableName);
    void logTableAlter(const std::string& username, const std::string& databaseName, const std::string& tableName);
    
    // Log index operations
    void logIndexCreate(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& indexName);
    void logIndexDrop(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& indexName);
    
    // Log data operations
    void logDataInsert(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& recordId = "");
    void logDataSelect(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& condition = "");
    void logDataUpdate(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& recordId = "");
    void logDataDelete(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& recordId = "");
    
    // Log transaction operations
    void logTransactionStart(const std::string& username, const std::string& transactionId);
    void logTransactionCommit(const std::string& username, const std::string& transactionId);
    void logTransactionRollback(const std::string& username, const std::string& transactionId);
    
    // Log query execution
    void logQueryExecute(const std::string& username, const std::string& databaseName, const std::string& query, double executionTimeMs = 0.0);
    
    // Log permission operations
    void logPermissionGranted(const std::string& adminUser, const std::string& targetUser, const std::string& permission);
    void logPermissionRevoked(const std::string& adminUser, const std::string& targetUser, const std::string& permission);
    
    // Log role operations
    void logRoleAssigned(const std::string& adminUser, const std::string& targetUser, const std::string& role);
    void logRoleRevoked(const std::string& adminUser, const std::string& targetUser, const std::string& role);
    
    // Get audit events for a user
    std::vector<AuditEvent> getEventsForUser(const std::string& username) const;
    
    // Get audit events for a database
    std::vector<AuditEvent> getEventsForDatabase(const std::string& databaseName) const;
    
    // Get audit events for a time range
    std::vector<AuditEvent> getEventsForTimeRange(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) const;
    
    // Get recent audit events
    std::vector<AuditEvent> getRecentEvents(size_t count = 100) const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace audit
} // namespace phantomdb

#endif // PHANTOMDB_AUDIT_LOGGER_H#ifndef PHANTOMDB_AUDIT_LOGGER_H
#define PHANTOMDB_AUDIT_LOGGER_H

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <unordered_map>

namespace phantomdb {
namespace audit {

// Audit event types
enum class AuditEventType {
    USER_LOGIN,
    USER_LOGOUT,
    DATABASE_CREATE,
    DATABASE_DROP,
    TABLE_CREATE,
    TABLE_DROP,
    TABLE_ALTER,
    INDEX_CREATE,
    INDEX_DROP,
    DATA_INSERT,
    DATA_SELECT,
    DATA_UPDATE,
    DATA_DELETE,
    TRANSACTION_START,
    TRANSACTION_COMMIT,
    TRANSACTION_ROLLBACK,
    QUERY_EXECUTE,
    PERMISSION_GRANTED,
    PERMISSION_REVOKED,
    ROLE_ASSIGNED,
    ROLE_REVOKED
};

// Audit event structure
struct AuditEvent {
    std::chrono::system_clock::time_point timestamp;
    std::string username;
    std::string database;
    std::string table;
    AuditEventType eventType;
    std::string details;
    std::string ipAddress;
};

class AuditLogger {
public:
    AuditLogger();
    ~AuditLogger();
    
    // Initialize the audit logger
    bool initialize(const std::string& logFilePath = "audit.log");
    
    // Shutdown the audit logger
    void shutdown();
    
    // Log an audit event
    void logEvent(const AuditEvent& event);
    
    // Log a user login
    void logUserLogin(const std::string& username, const std::string& ipAddress = "");
    
    // Log a user logout
    void logUserLogout(const std::string& username, const std::string& ipAddress = "");
    
    // Log database operations
    void logDatabaseCreate(const std::string& username, const std::string& databaseName);
    void logDatabaseDrop(const std::string& username, const std::string& databaseName);
    
    // Log table operations
    void logTableCreate(const std::string& username, const std::string& databaseName, const std::string& tableName);
    void logTableDrop(const std::string& username, const std::string& databaseName, const std::string& tableName);
    void logTableAlter(const std::string& username, const std::string& databaseName, const std::string& tableName);
    
    // Log index operations
    void logIndexCreate(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& indexName);
    void logIndexDrop(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& indexName);
    
    // Log data operations
    void logDataInsert(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& recordId = "");
    void logDataSelect(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& condition = "");
    void logDataUpdate(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& recordId = "");
    void logDataDelete(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& recordId = "");
    
    // Log transaction operations
    void logTransactionStart(const std::string& username, const std::string& transactionId);
    void logTransactionCommit(const std::string& username, const std::string& transactionId);
    void logTransactionRollback(const std::string& username, const std::string& transactionId);
    
    // Log query execution
    void logQueryExecute(const std::string& username, const std::string& databaseName, const std::string& query, double executionTimeMs = 0.0);
    
    // Log permission operations
    void logPermissionGranted(const std::string& adminUser, const std::string& targetUser, const std::string& permission);
    void logPermissionRevoked(const std::string& adminUser, const std::string& targetUser, const std::string& permission);
    
    // Log role operations
    void logRoleAssigned(const std::string& adminUser, const std::string& targetUser, const std::string& role);
    void logRoleRevoked(const std::string& adminUser, const std::string& targetUser, const std::string& role);
    
    // Get audit events for a user
    std::vector<AuditEvent> getEventsForUser(const std::string& username) const;
    
    // Get audit events for a database
    std::vector<AuditEvent> getEventsForDatabase(const std::string& databaseName) const;
    
    // Get audit events for a time range
    std::vector<AuditEvent> getEventsForTimeRange(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) const;
    
    // Get recent audit events
    std::vector<AuditEvent> getRecentEvents(size_t count = 100) const;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace audit
} // namespace phantomdb

#endif // PHANTOMDB_AUDIT_LOGGER_H