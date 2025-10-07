#include "error_handler.h"
#include <iostream>

using namespace phantomdb::error;

int main() {
    std::cout << "PhantomDB Error Handling Example" << std::endl;
    std::cout << "================================" << std::endl;
    
    // Example 1: Database not found error
    try {
        PhantomDBError dbError = ErrorHandler::createError(
            ErrorCode::DATABASE_NOT_FOUND,
            "Database 'production' not found",
            "Check if the database exists or create it using CREATE DATABASE"
        );
        
        ErrorHandler::logError(dbError);
        std::cout << "Error: " << dbError.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    std::cout << std::endl;
    
    // Example 2: Syntax error
    try {
        PhantomDBError syntaxError = ErrorHandler::createError(
            ErrorCode::SYNTAX_ERROR,
            "Unexpected token 'SELECT' at position 15",
            "Review the SQL query syntax and correct the error"
        );
        
        ErrorHandler::logError(syntaxError);
        std::cout << "Error: " << syntaxError.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    std::cout << std::endl;
    
    // Example 3: Critical error
    try {
        PhantomDBError criticalError = ErrorHandler::createError(
            ErrorCode::DATABASE_ALREADY_EXISTS,
            "Database 'testdb' already exists",
            "Choose a different database name or drop the existing database",
            ErrorSeverity::CRITICAL
        );
        
        ErrorHandler::logError(criticalError);
        std::cout << "Error: " << criticalError.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    std::cout << std::endl;
    
    // Example 4: Using error codes directly
    std::cout << "Error Code Examples:" << std::endl;
    std::cout << "  " << ErrorHandler::getErrorName(ErrorCode::SUCCESS) << ": " 
              << ErrorHandler::getErrorDescription(ErrorCode::SUCCESS) << std::endl;
    std::cout << "  " << ErrorHandler::getErrorName(ErrorCode::TABLE_NOT_FOUND) << ": " 
              << ErrorHandler::getErrorDescription(ErrorCode::TABLE_NOT_FOUND) << std::endl;
    std::cout << "  " << ErrorHandler::getErrorName(ErrorCode::AUTHENTICATION_FAILED) << ": " 
              << ErrorHandler::getErrorDescription(ErrorCode::AUTHENTICATION_FAILED) << std::endl;
    
    return 0;
}