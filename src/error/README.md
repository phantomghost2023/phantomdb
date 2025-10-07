# PhantomDB Error Handling Module

Structured error reporting with codes and remediation hints for PhantomDB.

## Features

- Comprehensive error code system with categories
- Structured error objects with detailed information
- Error severity levels (INFO, WARNING, ERROR, CRITICAL)
- Remediation hints for common errors
- Thread-safe error handler singleton
- Easy integration with existing codebase

## Error Code Categories

- **General Errors** (1000-1999): System-level errors
- **Database Errors** (2000-2999): Database management errors
- **Table Errors** (3000-3999): Table-related errors
- **Column Errors** (4000-4999): Column-related errors
- **Index Errors** (5000-5999): Index-related errors
- **Query Errors** (6000-6999): Query processing errors
- **Transaction Errors** (7000-7999): Transaction management errors
- **Security Errors** (8000-8999): Authentication and authorization errors
- **File I/O Errors** (9000-9999): File system errors
- **Network Errors** (10000+): Network communication errors

## Usage

### Creating Errors

```cpp
#include "error_handler.h"

using namespace phantomdb::error;

// Create a simple error
PhantomDBError error = ErrorHandler::createError(
    ErrorCode::DATABASE_NOT_FOUND,
    "Database 'testdb' not found"
);

// Create an error with remediation hint
PhantomDBError errorWithRemediation = ErrorHandler::createError(
    ErrorCode::SYNTAX_ERROR,
    "Unexpected token 'FROM' in query",
    "Check the SQL syntax and correct the query"
);

// Create an error with custom severity
PhantomDBError criticalError = ErrorHandler::createError(
    ErrorCode::DATABASE_ALREADY_EXISTS,
    "Database 'testdb' already exists",
    ErrorSeverity::CRITICAL,
    "Choose a different database name or drop the existing database"
);
```

### Handling Errors

```cpp
try {
    // Some operation that might fail
    if (databaseNotFound) {
        throw ErrorHandler::createError(
            ErrorCode::DATABASE_NOT_FOUND,
            "Database 'testdb' not found",
            "Create the database using CREATE DATABASE statement"
        );
    }
} catch (const PhantomDBError& e) {
    // Log the error
    ErrorHandler::logError(e);
    
    // Handle based on severity
    switch (e.getSeverity()) {
        case ErrorSeverity::INFO:
            std::cout << "Info: " << e.what() << std::endl;
            break;
        case ErrorSeverity::WARNING:
            std::cerr << "Warning: " << e.what() << std::endl;
            break;
        case ErrorSeverity::ERROR:
            std::cerr << "Error: " << e.what() << std::endl;
            // Handle error
            break;
        case ErrorSeverity::CRITICAL:
            std::cerr << "Critical: " << e.what() << std::endl;
            // Terminate or take critical action
            break;
    }
}
```

### Error Information

```cpp
// Get error name
std::string name = ErrorHandler::getErrorName(ErrorCode::SYNTAX_ERROR);
// Returns: "SYNTAX_ERROR"

// Get error description
std::string description = ErrorHandler::getErrorDescription(ErrorCode::SYNTAX_ERROR);
// Returns: "The query contains syntax errors"

// Get default severity for an error code
ErrorSeverity severity = ErrorHandler::getDefaultSeverity(ErrorCode::DATABASE_NOT_FOUND);
// Returns: ErrorSeverity::WARNING
```

## Integration

To integrate the error handling module into your code:

1. Include the header: `#include "error_handler.h"`
2. Link with the error library in your CMakeLists.txt
3. Use `ErrorHandler::createError()` to create structured errors
4. Use `ErrorHandler::logError()` to log errors
5. Handle errors based on their severity levels

## Future Enhancements

- JSON error serialization for API responses
- Internationalization support for error messages
- Error chaining for nested operations
- Performance monitoring integration
- Custom error categories for extensions