#include "error_handler.h"
#include <iostream>
#include <cassert>
#include <mutex>

using namespace phantomdb::error;

void testErrorCreation() {
    std::cout << "Testing error creation..." << std::endl;
    
    // Test basic error creation
    PhantomDBError error = ErrorHandler::createError(
        ErrorCode::DATABASE_NOT_FOUND,
        "Database 'testdb' not found",
        "Create the database using CREATE DATABASE statement"
    );
    
    assert(error.getCode() == ErrorCode::DATABASE_NOT_FOUND);
    assert(error.getMessage() == "Database 'testdb' not found");
    assert(error.getRemediation() == "Create the database using CREATE DATABASE statement");
    assert(error.getSeverity() == ErrorSeverity::WARNING);
    
    std::cout << "Error creation test passed!" << std::endl;
}

void testErrorToString() {
    std::cout << "Testing error to string conversion..." << std::endl;
    
    PhantomDBError error = ErrorHandler::createError(
        ErrorCode::SYNTAX_ERROR,
        "Unexpected token 'FROM' in query",
        "Check the SQL syntax and correct the query"
    );
    
    std::string errorString = error.toString();
    assert(errorString.find("[SYNTAX_ERROR]") != std::string::npos);
    assert(errorString.find("Unexpected token 'FROM' in query") != std::string::npos);
    assert(errorString.find("Check the SQL syntax and correct the query") != std::string::npos);
    
    std::cout << "Error to string conversion test passed!" << std::endl;
}

void testErrorWhat() {
    std::cout << "Testing error what() method..." << std::endl;
    
    PhantomDBError error = ErrorHandler::createError(
        ErrorCode::TABLE_NOT_FOUND,
        "Table 'users' not found in database 'testdb'"
    );
    
    const char* whatMessage = error.what();
    assert(std::string(whatMessage).find("[TABLE_NOT_FOUND]") != std::string::npos);
    assert(std::string(whatMessage).find("Table 'users' not found in database 'testdb'") != std::string::npos);
    
    std::cout << "Error what() method test passed!" << std::endl;
}

void testErrorNames() {
    std::cout << "Testing error name retrieval..." << std::endl;
    
    assert(ErrorHandler::getErrorName(ErrorCode::SUCCESS) == "SUCCESS");
    assert(ErrorHandler::getErrorName(ErrorCode::DATABASE_NOT_FOUND) == "DATABASE_NOT_FOUND");
    assert(ErrorHandler::getErrorName(ErrorCode::SYNTAX_ERROR) == "SYNTAX_ERROR");
    assert(ErrorHandler::getErrorName(ErrorCode::UNKNOWN_ERROR) == "UNKNOWN_ERROR");
    
    std::cout << "Error name retrieval test passed!" << std::endl;
}

void testErrorDescriptions() {
    std::cout << "Testing error description retrieval..." << std::endl;
    
    assert(ErrorHandler::getErrorDescription(ErrorCode::SUCCESS) == "Operation completed successfully");
    assert(ErrorHandler::getErrorDescription(ErrorCode::DATABASE_NOT_FOUND) == "The specified database was not found");
    assert(ErrorHandler::getErrorDescription(ErrorCode::SYNTAX_ERROR) == "The query contains syntax errors");
    
    std::cout << "Error description retrieval test passed!" << std::endl;
}

void testErrorSeverity() {
    std::cout << "Testing error severity..." << std::endl;
    
    assert(ErrorHandler::getDefaultSeverity(ErrorCode::SUCCESS) == ErrorSeverity::INFO);
    assert(ErrorHandler::getDefaultSeverity(ErrorCode::DATABASE_NOT_FOUND) == ErrorSeverity::WARNING);
    assert(ErrorHandler::getDefaultSeverity(ErrorCode::DATABASE_CONNECTION_FAILED) == ErrorSeverity::ERROR);
    assert(ErrorHandler::getDefaultSeverity(ErrorCode::DATABASE_ALREADY_EXISTS) == ErrorSeverity::CRITICAL);
    
    std::cout << "Error severity test passed!" << std::endl;
}

int main() {
    std::cout << "Running PhantomDB Error Handler tests..." << std::endl;
    
    testErrorCreation();
    testErrorToString();
    testErrorWhat();
    testErrorNames();
    testErrorDescriptions();
    testErrorSeverity();
    
    std::cout << "All PhantomDB Error Handler tests passed!" << std::endl;
    return 0;
}