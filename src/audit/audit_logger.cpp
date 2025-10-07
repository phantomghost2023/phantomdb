#include "audit_logger.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <mutex>
#include <chrono>

namespace phantomdb {
namespace audit {

class AuditLogger::Impl {
public:
    Impl() : enabled_(false) {}
    ~Impl() {
        if (logFile_.is_open()) {
            logFile_.close();
        }
    }
    
    bool initialize(const std::string& logFilePath) {
        logFilePath_ = logFilePath;
        logFile_.open(logFilePath_, std::ios::app);
        if (!logFile_.is_open()) {
            std::cerr << "Failed to open audit log file: " << logFilePath_ << std::endl;
            return false;
        }
        
        enabled_ = true;
        std::cout << "Audit logger initialized with log file: " << logFilePath_ << std::endl;
        return true;
    }
    
    void shutdown() {
        if (logFile_.is_open()) {
            logFile_.close();
        }
        enabled_ = false;
        std::cout << "Audit logger shutdown" << std::endl;
    }
    
    void logEvent(const AuditEvent& event) {
        if (!enabled_) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Format timestamp
        auto time_t = std::chrono::system_clock::to_time_t(event.timestamp);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            event.timestamp.time_since_epoch()) % 1000;
        
        std::ostringstream timestampStr;
        timestampStr << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        timestampStr << '.' << std::setfill('0') << std::setw(3) << ms.count();
        
        // Format event type
        std::string eventTypeStr = getEventTypeString(event.eventType);
        
        // Write to log file
        logFile_ << "[" << timestampStr.str() << "] "
                 << "[" << eventTypeStr << "] "
                 << "User: " << event.username << " "
                 << "DB: " << event.database << " "
                 << "Table: " << event.table << " "
                 << "IP: " << event.ipAddress << " "
                 << "Details: " << event.details << std::endl;
        
        logFile_.flush();
        
        // Store in memory for querying
        events_.push_back(event);
        
        // Keep only the most recent 10000 events in memory
        if (events_.size() > 10000) {
            events_.erase(events_.begin(), events_.begin() + 1000);
        }
    }
    
    std::vector<AuditEvent> getEventsForUser(const std::string& username) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<AuditEvent> result;
        
        for (const auto& event : events_) {
            if (event.username == username) {
                result.push_back(event);
            }
        }
        
        return result;
    }
    
    std::vector<AuditEvent> getEventsForDatabase(const std::string& databaseName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<AuditEvent> result;
        
        for (const auto& event : events_) {
            if (event.database == databaseName) {
                result.push_back(event);
            }
        }
        
        return result;
    }
    
    std::vector<AuditEvent> getEventsForTimeRange(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<AuditEvent> result;
        
        for (const auto& event : events_) {
            if (event.timestamp >= start && event.timestamp <= end) {
                result.push_back(event);
            }
        }
        
        return result;
    }
    
    std::vector<AuditEvent> getRecentEvents(size_t count) const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (events_.empty()) {
            return {};
        }
        
        size_t actualCount = std::min(count, events_.size());
        std::vector<AuditEvent> result;
        result.reserve(actualCount);
        
        auto startIt = events_.end() - actualCount;
        result.insert(result.end(), startIt, events_.end());
        
        return result;
    }
    
private:
    bool enabled_;
    std::string logFilePath_;
    std::ofstream logFile_;
    std::vector<AuditEvent> events_;
    mutable std::mutex mutex_;
    
    std::string getEventTypeString(AuditEventType eventType) const {
        switch (eventType) {
            case AuditEventType::USER_LOGIN: return "USER_LOGIN";
            case AuditEventType::USER_LOGOUT: return "USER_LOGOUT";
            case AuditEventType::DATABASE_CREATE: return "DATABASE_CREATE";
            case AuditEventType::DATABASE_DROP: return "DATABASE_DROP";
            case AuditEventType::TABLE_CREATE: return "TABLE_CREATE";
            case AuditEventType::TABLE_DROP: return "TABLE_DROP";
            case AuditEventType::TABLE_ALTER: return "TABLE_ALTER";
            case AuditEventType::INDEX_CREATE: return "INDEX_CREATE";
            case AuditEventType::INDEX_DROP: return "INDEX_DROP";
            case AuditEventType::DATA_INSERT: return "DATA_INSERT";
            case AuditEventType::DATA_SELECT: return "DATA_SELECT";
            case AuditEventType::DATA_UPDATE: return "DATA_UPDATE";
            case AuditEventType::DATA_DELETE: return "DATA_DELETE";
            case AuditEventType::TRANSACTION_START: return "TRANSACTION_START";
            case AuditEventType::TRANSACTION_COMMIT: return "TRANSACTION_COMMIT";
            case AuditEventType::TRANSACTION_ROLLBACK: return "TRANSACTION_ROLLBACK";
            case AuditEventType::QUERY_EXECUTE: return "QUERY_EXECUTE";
            case AuditEventType::PERMISSION_GRANTED: return "PERMISSION_GRANTED";
            case AuditEventType::PERMISSION_REVOKED: return "PERMISSION_REVOKED";
            case AuditEventType::ROLE_ASSIGNED: return "ROLE_ASSIGNED";
            case AuditEventType::ROLE_REVOKED: return "ROLE_REVOKED";
            default: return "UNKNOWN";
        }
    }
};

AuditLogger::AuditLogger() : pImpl(std::make_unique<Impl>()) {
    std::cout << "Audit logger created" << std::endl;
}

AuditLogger::~AuditLogger() {
    std::cout << "Audit logger destroyed" << std::endl;
}

bool AuditLogger::initialize(const std::string& logFilePath) {
    return pImpl->initialize(logFilePath);
}

void AuditLogger::shutdown() {
    pImpl->shutdown();
}

void AuditLogger::logEvent(const AuditEvent& event) {
    pImpl->logEvent(event);
}

void AuditLogger::logUserLogin(const std::string& username, const std::string& ipAddress) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = username;
    event.ipAddress = ipAddress;
    event.eventType = AuditEventType::USER_LOGIN;
    event.details = "User logged in";
    logEvent(event);
}

void AuditLogger::logUserLogout(const std::string& username, const std::string& ipAddress) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = username;
    event.ipAddress = ipAddress;
    event.eventType = AuditEventType::USER_LOGOUT;
    event.details = "User logged out";
    logEvent(event);
}

void AuditLogger::logDatabaseCreate(const std::string& username, const std::string& databaseName) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = username;
    event.database = databaseName;
    event.eventType = AuditEventType::DATABASE_CREATE;
    event.details = "Database created";
    logEvent(event);
}

void AuditLogger::logDatabaseDrop(const std::string& username, const std::string& databaseName) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = username;
    event.database = databaseName;
    event.eventType = AuditEventType::DATABASE_DROP;
    event.details = "Database dropped";
    logEvent(event);
}

void AuditLogger::logTableCreate(const std::string& username, const std::string& databaseName, const std::string& tableName) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = username;
    event.database = databaseName;
    event.table = tableName;
    event.eventType = AuditEventType::TABLE_CREATE;
    event.details = "Table created";
    logEvent(event);
}

void AuditLogger::logTableDrop(const std::string& username, const std::string& databaseName, const std::string& tableName) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = username;
    event.database = databaseName;
    event.table = tableName;
    event.eventType = AuditEventType::TABLE_DROP;
    event.details = "Table dropped";
    logEvent(event);
}

void AuditLogger::logTableAlter(const std::string& username, const std::string& databaseName, const std::string& tableName) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = username;
    event.database = databaseName;
    event.table = tableName;
    event.eventType = AuditEventType::TABLE_ALTER;
    event.details = "Table altered";
    logEvent(event);
}

void AuditLogger::logIndexCreate(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& indexName) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = username;
    event.database = databaseName;
    event.table = tableName;
    event.eventType = AuditEventType::INDEX_CREATE;
    event.details = "Index " + indexName + " created";
    logEvent(event);
}

void AuditLogger::logIndexDrop(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& indexName) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = username;
    event.database = databaseName;
    event.table = tableName;
    event.eventType = AuditEventType::INDEX_DROP;
    event.details = "Index " + indexName + " dropped";
    logEvent(event);
}

void AuditLogger::logDataInsert(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& recordId) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = username;
    event.database = databaseName;
    event.table = tableName;
    event.eventType = AuditEventType::DATA_INSERT;
    event.details = "Record inserted" + (recordId.empty() ? "" : " (ID: " + recordId + ")");
    logEvent(event);
}

void AuditLogger::logDataSelect(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& condition) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = username;
    event.database = databaseName;
    event.table = tableName;
    event.eventType = AuditEventType::DATA_SELECT;
    event.details = "Data selected" + (condition.empty() ? "" : " (Condition: " + condition + ")");
    logEvent(event);
}

void AuditLogger::logDataUpdate(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& recordId) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = username;
    event.database = databaseName;
    event.table = tableName;
    event.eventType = AuditEventType::DATA_UPDATE;
    event.details = "Record updated" + (recordId.empty() ? "" : " (ID: " + recordId + ")");
    logEvent(event);
}

void AuditLogger::logDataDelete(const std::string& username, const std::string& databaseName, const std::string& tableName, const std::string& recordId) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = username;
    event.database = databaseName;
    event.table = tableName;
    event.eventType = AuditEventType::DATA_DELETE;
    event.details = "Record deleted" + (recordId.empty() ? "" : " (ID: " + recordId + ")");
    logEvent(event);
}

void AuditLogger::logTransactionStart(const std::string& username, const std::string& transactionId) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = username;
    event.eventType = AuditEventType::TRANSACTION_START;
    event.details = "Transaction started (ID: " + transactionId + ")";
    logEvent(event);
}

void AuditLogger::logTransactionCommit(const std::string& username, const std::string& transactionId) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = username;
    event.eventType = AuditEventType::TRANSACTION_COMMIT;
    event.details = "Transaction committed (ID: " + transactionId + ")";
    logEvent(event);
}

void AuditLogger::logTransactionRollback(const std::string& username, const std::string& transactionId) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = username;
    event.eventType = AuditEventType::TRANSACTION_ROLLBACK;
    event.details = "Transaction rolled back (ID: " + transactionId + ")";
    logEvent(event);
}

void AuditLogger::logQueryExecute(const std::string& username, const std::string& databaseName, const std::string& query, double executionTimeMs) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = username;
    event.database = databaseName;
    event.eventType = AuditEventType::QUERY_EXECUTE;
    event.details = "Query executed: " + query + 
                   (executionTimeMs > 0 ? " (Time: " + std::to_string(executionTimeMs) + "ms)" : "");
    logEvent(event);
}

void AuditLogger::logPermissionGranted(const std::string& adminUser, const std::string& targetUser, const std::string& permission) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = adminUser;
    event.eventType = AuditEventType::PERMISSION_GRANTED;
    event.details = "Permission " + permission + " granted to user " + targetUser;
    logEvent(event);
}

void AuditLogger::logPermissionRevoked(const std::string& adminUser, const std::string& targetUser, const std::string& permission) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = adminUser;
    event.eventType = AuditEventType::PERMISSION_REVOKED;
    event.details = "Permission " + permission + " revoked from user " + targetUser;
    logEvent(event);
}

void AuditLogger::logRoleAssigned(const std::string& adminUser, const std::string& targetUser, const std::string& role) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = adminUser;
    event.eventType = AuditEventType::ROLE_ASSIGNED;
    event.details = "Role " + role + " assigned to user " + targetUser;
    logEvent(event);
}

void AuditLogger::logRoleRevoked(const std::string& adminUser, const std::string& targetUser, const std::string& role) {
    AuditEvent event;
    event.timestamp = std::chrono::system_clock::now();
    event.username = adminUser;
    event.eventType = AuditEventType::ROLE_REVOKED;
    event.details = "Role " + role + " revoked from user " + targetUser;
    logEvent(event);
}

std::vector<AuditEvent> AuditLogger::getEventsForUser(const std::string& username) const {
    return pImpl->getEventsForUser(username);
}

std::vector<AuditEvent> AuditLogger::getEventsForDatabase(const std::string& databaseName) const {
    return pImpl->getEventsForDatabase(databaseName);
}

std::vector<AuditEvent> AuditLogger::getEventsForTimeRange(
    const std::chrono::system_clock::time_point& start,
    const std::chrono::system_clock::time_point& end) const {
    return pImpl->getEventsForTimeRange(start, end);
}

std::vector<AuditEvent> AuditLogger::getRecentEvents(size_t count) const {
    return pImpl->getRecentEvents(count);
}

} // namespace audit
} // namespace phantomdb