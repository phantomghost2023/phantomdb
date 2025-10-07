#include "error_handler.h"
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <mutex>

namespace phantomdb {
namespace error {

// Static member definitions
std::unique_ptr<ErrorHandler> ErrorHandler::instance_;
std::mutex ErrorHandler::instance_mutex_;

// PhantomDBError implementation
PhantomDBError::PhantomDBError(ErrorCode code, const std::string& message, const std::string& remediation)
    : PhantomDBError(code, message, ErrorSeverity::ERROR, remediation) {
}

PhantomDBError::PhantomDBError(ErrorCode code, const std::string& message, ErrorSeverity severity, const std::string& remediation)
    : code_(code), message_(message), severity_(severity), remediation_(remediation) {
}

ErrorCode PhantomDBError::getCode() const {
    return code_;
}

const std::string& PhantomDBError::getMessage() const {
    return message_;
}

ErrorSeverity PhantomDBError::getSeverity() const {
    return severity_;
}

const std::string& PhantomDBError::getRemediation() const {
    return remediation_;
}

const char* PhantomDBError::what() const noexcept {
    if (what_message_.empty()) {
        what_message_ = toString();
    }
    return what_message_.c_str();
}

std::string PhantomDBError::toString() const {
    std::ostringstream oss;
    oss << "[" << ErrorHandler::getErrorName(code_) << "] ";
    oss << message_;
    
    if (!remediation_.empty()) {
        oss << " (Remediation: " << remediation_ << ")";
    }
    
    return oss.str();
}

// ErrorHandler implementation
ErrorHandler::ErrorHandler() {
}

ErrorHandler::~ErrorHandler() {
}

ErrorHandler& ErrorHandler::getInstance() {
    std::lock_guard<std::mutex> lock(instance_mutex_);
    if (!instance_) {
        instance_ = std::unique_ptr<ErrorHandler>(new ErrorHandler());
    }
    return *instance_;
}

PhantomDBError ErrorHandler::createError(ErrorCode code, const std::string& message, const std::string& remediation) {
    return PhantomDBError(code, message, remediation);
}

PhantomDBError ErrorHandler::createError(ErrorCode code, const std::string& message, ErrorSeverity severity, const std::string& remediation) {
    return PhantomDBError(code, message, severity, remediation);
}

std::string ErrorHandler::getErrorName(ErrorCode code) {
    switch (code) {
        case ErrorCode::SUCCESS: return "SUCCESS";
        case ErrorCode::UNKNOWN_ERROR: return "UNKNOWN_ERROR";
        case ErrorCode::NOT_IMPLEMENTED: return "NOT_IMPLEMENTED";
        case ErrorCode::INVALID_OPERATION: return "INVALID_OPERATION";
        
        case ErrorCode::DATABASE_NOT_FOUND: return "DATABASE_NOT_FOUND";
        case ErrorCode::DATABASE_ALREADY_EXISTS: return "DATABASE_ALREADY_EXISTS";
        case ErrorCode::DATABASE_CONNECTION_FAILED: return "DATABASE_CONNECTION_FAILED";
        
        case ErrorCode::TABLE_NOT_FOUND: return "TABLE_NOT_FOUND";
        case ErrorCode::TABLE_ALREADY_EXISTS: return "TABLE_ALREADY_EXISTS";
        case ErrorCode::TABLE_CREATION_FAILED: return "TABLE_CREATION_FAILED";
        
        case ErrorCode::COLUMN_NOT_FOUND: return "COLUMN_NOT_FOUND";
        case ErrorCode::COLUMN_ALREADY_EXISTS: return "COLUMN_ALREADY_EXISTS";
        case ErrorCode::INVALID_COLUMN_TYPE: return "INVALID_COLUMN_TYPE";
        
        case ErrorCode::INDEX_NOT_FOUND: return "INDEX_NOT_FOUND";
        case ErrorCode::INDEX_ALREADY_EXISTS: return "INDEX_ALREADY_EXISTS";
        case ErrorCode::INDEX_CREATION_FAILED: return "INDEX_CREATION_FAILED";
        
        case ErrorCode::SYNTAX_ERROR: return "SYNTAX_ERROR";
        case ErrorCode::INVALID_QUERY: return "INVALID_QUERY";
        case ErrorCode::QUERY_EXECUTION_FAILED: return "QUERY_EXECUTION_FAILED";
        
        case ErrorCode::TRANSACTION_NOT_FOUND: return "TRANSACTION_NOT_FOUND";
        case ErrorCode::TRANSACTION_ALREADY_EXISTS: return "TRANSACTION_ALREADY_EXISTS";
        case ErrorCode::TRANSACTION_COMMIT_FAILED: return "TRANSACTION_COMMIT_FAILED";
        case ErrorCode::TRANSACTION_ROLLBACK_FAILED: return "TRANSACTION_ROLLBACK_FAILED";
        
        case ErrorCode::AUTHENTICATION_FAILED: return "AUTHENTICATION_FAILED";
        case ErrorCode::AUTHORIZATION_FAILED: return "AUTHORIZATION_FAILED";
        case ErrorCode::INVALID_PERMISSION: return "INVALID_PERMISSION";
        
        case ErrorCode::FILE_NOT_FOUND: return "FILE_NOT_FOUND";
        case ErrorCode::FILE_ACCESS_DENIED: return "FILE_ACCESS_DENIED";
        case ErrorCode::FILE_CORRUPTED: return "FILE_CORRUPTED";
        
        case ErrorCode::NETWORK_CONNECTION_FAILED: return "NETWORK_CONNECTION_FAILED";
        case ErrorCode::NETWORK_TIMEOUT: return "NETWORK_TIMEOUT";
        case ErrorCode::NETWORK_UNREACHABLE: return "NETWORK_UNREACHABLE";
        
        default: return "UNKNOWN_ERROR_CODE";
    }
}

std::string ErrorHandler::getErrorDescription(ErrorCode code) {
    switch (code) {
        case ErrorCode::SUCCESS: return "Operation completed successfully";
        case ErrorCode::UNKNOWN_ERROR: return "An unknown error occurred";
        case ErrorCode::NOT_IMPLEMENTED: return "The requested functionality is not implemented";
        case ErrorCode::INVALID_OPERATION: return "The requested operation is invalid";
        
        case ErrorCode::DATABASE_NOT_FOUND: return "The specified database was not found";
        case ErrorCode::DATABASE_ALREADY_EXISTS: return "A database with the specified name already exists";
        case ErrorCode::DATABASE_CONNECTION_FAILED: return "Failed to connect to the database";
        
        case ErrorCode::TABLE_NOT_FOUND: return "The specified table was not found";
        case ErrorCode::TABLE_ALREADY_EXISTS: return "A table with the specified name already exists";
        case ErrorCode::TABLE_CREATION_FAILED: return "Failed to create the table";
        
        case ErrorCode::COLUMN_NOT_FOUND: return "The specified column was not found";
        case ErrorCode::COLUMN_ALREADY_EXISTS: return "A column with the specified name already exists";
        case ErrorCode::INVALID_COLUMN_TYPE: return "The specified column type is invalid";
        
        case ErrorCode::INDEX_NOT_FOUND: return "The specified index was not found";
        case ErrorCode::INDEX_ALREADY_EXISTS: return "An index with the specified name already exists";
        case ErrorCode::INDEX_CREATION_FAILED: return "Failed to create the index";
        
        case ErrorCode::SYNTAX_ERROR: return "The query contains syntax errors";
        case ErrorCode::INVALID_QUERY: return "The query is invalid";
        case ErrorCode::QUERY_EXECUTION_FAILED: return "Failed to execute the query";
        
        case ErrorCode::TRANSACTION_NOT_FOUND: return "The specified transaction was not found";
        case ErrorCode::TRANSACTION_ALREADY_EXISTS: return "A transaction with the specified ID already exists";
        case ErrorCode::TRANSACTION_COMMIT_FAILED: return "Failed to commit the transaction";
        case ErrorCode::TRANSACTION_ROLLBACK_FAILED: return "Failed to rollback the transaction";
        
        case ErrorCode::AUTHENTICATION_FAILED: return "Authentication failed";
        case ErrorCode::AUTHORIZATION_FAILED: return "Authorization failed";
        case ErrorCode::INVALID_PERMISSION: return "Invalid permission";
        
        case ErrorCode::FILE_NOT_FOUND: return "The specified file was not found";
        case ErrorCode::FILE_ACCESS_DENIED: return "Access to the file was denied";
        case ErrorCode::FILE_CORRUPTED: return "The file is corrupted";
        
        case ErrorCode::NETWORK_CONNECTION_FAILED: return "Network connection failed";
        case ErrorCode::NETWORK_TIMEOUT: return "Network operation timed out";
        case ErrorCode::NETWORK_UNREACHABLE: return "Network destination is unreachable";
        
        default: return "No description available for this error code";
    }
}

ErrorSeverity ErrorHandler::getDefaultSeverity(ErrorCode code) {
    switch (code) {
        case ErrorCode::SUCCESS:
            return ErrorSeverity::INFO;
            
        case ErrorCode::NOT_IMPLEMENTED:
        case ErrorCode::INVALID_OPERATION:
        case ErrorCode::TABLE_NOT_FOUND:
        case ErrorCode::COLUMN_NOT_FOUND:
        case ErrorCode::INDEX_NOT_FOUND:
        case ErrorCode::SYNTAX_ERROR:
        case ErrorCode::INVALID_QUERY:
        case ErrorCode::AUTHENTICATION_FAILED:
        case ErrorCode::INVALID_PERMISSION:
        case ErrorCode::FILE_NOT_FOUND:
            return ErrorSeverity::WARNING;
            
        case ErrorCode::DATABASE_NOT_FOUND:
        case ErrorCode::DATABASE_CONNECTION_FAILED:
        case ErrorCode::TABLE_CREATION_FAILED:
        case ErrorCode::INDEX_CREATION_FAILED:
        case ErrorCode::QUERY_EXECUTION_FAILED:
        case ErrorCode::TRANSACTION_COMMIT_FAILED:
        case ErrorCode::TRANSACTION_ROLLBACK_FAILED:
        case ErrorCode::AUTHORIZATION_FAILED:
        case ErrorCode::FILE_ACCESS_DENIED:
        case ErrorCode::FILE_CORRUPTED:
        case ErrorCode::NETWORK_CONNECTION_FAILED:
        case ErrorCode::NETWORK_TIMEOUT:
        case ErrorCode::NETWORK_UNREACHABLE:
            return ErrorSeverity::ERROR;
            
        case ErrorCode::UNKNOWN_ERROR:
        case ErrorCode::DATABASE_ALREADY_EXISTS:
        case ErrorCode::TABLE_ALREADY_EXISTS:
        case ErrorCode::COLUMN_ALREADY_EXISTS:
        case ErrorCode::INDEX_ALREADY_EXISTS:
        case ErrorCode::TRANSACTION_ALREADY_EXISTS:
        case ErrorCode::INVALID_COLUMN_TYPE:
            return ErrorSeverity::CRITICAL;
            
        default:
            return ErrorSeverity::ERROR;
    }
}

void ErrorHandler::logError(const PhantomDBError& error) {
    // In a real implementation, this would log to a file or logging system
    std::string severityStr;
    switch (error.getSeverity()) {
        case ErrorSeverity::INFO: severityStr = "INFO"; break;
        case ErrorSeverity::WARNING: severityStr = "WARN"; break;
        case ErrorSeverity::ERROR: severityStr = "ERROR"; break;
        case ErrorSeverity::CRITICAL: severityStr = "CRITICAL"; break;
    }
    
    std::cout << "[" << severityStr << "] " << error.toString() << std::endl;
}

} // namespace error
} // namespace phantomdb