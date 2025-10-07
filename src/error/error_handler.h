#ifndef PHANTOMDB_ERROR_HANDLER_H
#define PHANTOMDB_ERROR_HANDLER_H

#include <string>
#include <exception>
#include <memory>
#include <mutex>

namespace phantomdb {
namespace error {

// Error codes
enum class ErrorCode {
    // General errors
    SUCCESS = 0,
    UNKNOWN_ERROR = 1000,
    NOT_IMPLEMENTED = 1001,
    INVALID_OPERATION = 1002,
    
    // Database errors
    DATABASE_NOT_FOUND = 2000,
    DATABASE_ALREADY_EXISTS = 2001,
    DATABASE_CONNECTION_FAILED = 2002,
    
    // Table errors
    TABLE_NOT_FOUND = 3000,
    TABLE_ALREADY_EXISTS = 3001,
    TABLE_CREATION_FAILED = 3002,
    
    // Column errors
    COLUMN_NOT_FOUND = 4000,
    COLUMN_ALREADY_EXISTS = 4001,
    INVALID_COLUMN_TYPE = 4002,
    
    // Index errors
    INDEX_NOT_FOUND = 5000,
    INDEX_ALREADY_EXISTS = 5001,
    INDEX_CREATION_FAILED = 5002,
    
    // Query errors
    SYNTAX_ERROR = 6000,
    INVALID_QUERY = 6001,
    QUERY_EXECUTION_FAILED = 6002,
    
    // Transaction errors
    TRANSACTION_NOT_FOUND = 7000,
    TRANSACTION_ALREADY_EXISTS = 7001,
    TRANSACTION_COMMIT_FAILED = 7002,
    TRANSACTION_ROLLBACK_FAILED = 7003,
    
    // Security errors
    AUTHENTICATION_FAILED = 8000,
    AUTHORIZATION_FAILED = 8001,
    INVALID_PERMISSION = 8002,
    
    // File I/O errors
    FILE_NOT_FOUND = 9000,
    FILE_ACCESS_DENIED = 9001,
    FILE_CORRUPTED = 9002,
    
    // Network errors
    NETWORK_CONNECTION_FAILED = 10000,
    NETWORK_TIMEOUT = 10001,
    NETWORK_UNREACHABLE = 10002
};

// Error severity levels
enum class ErrorSeverity {
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

// Structured error class
class PhantomDBError : public std::exception {
public:
    PhantomDBError(ErrorCode code, const std::string& message, const std::string& remediation = "");
    PhantomDBError(ErrorCode code, const std::string& message, ErrorSeverity severity, const std::string& remediation = "");
    
    // Getters
    ErrorCode getCode() const;
    const std::string& getMessage() const;
    ErrorSeverity getSeverity() const;
    const std::string& getRemediation() const;
    
    // Override std::exception
    const char* what() const noexcept override;
    
    // Convert to string representation
    std::string toString() const;
    
private:
    ErrorCode code_;
    std::string message_;
    ErrorSeverity severity_;
    std::string remediation_;
    mutable std::string what_message_;
};

// Error handler class
class ErrorHandler {
public:
    static ErrorHandler& getInstance();
    
    // Create error objects
    static PhantomDBError createError(ErrorCode code, const std::string& message, const std::string& remediation = "");
    static PhantomDBError createError(ErrorCode code, const std::string& message, ErrorSeverity severity, const std::string& remediation = "");
    
    // Get error information
    static std::string getErrorName(ErrorCode code);
    static std::string getErrorDescription(ErrorCode code);
    static ErrorSeverity getDefaultSeverity(ErrorCode code);
    
    // Log error (placeholder for actual logging implementation)
    static void logError(const PhantomDBError& error);
    
private:
    ErrorHandler();
    ~ErrorHandler();
    
    // Singleton instance
    static std::unique_ptr<ErrorHandler> instance_;
    static std::mutex instance_mutex_;
};

} // namespace error
} // namespace phantomdb

#endif // PHANTOMDB_ERROR_HANDLER_H