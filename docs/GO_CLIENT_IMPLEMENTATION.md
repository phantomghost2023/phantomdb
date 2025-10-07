# Go Client Implementation

This document describes the implementation of the Go client library for PhantomDB.

## Overview

The Go client library provides a convenient way for Go developers to interact with PhantomDB through its REST API. The library is designed to be idiomatic Go code with proper error handling and efficient HTTP communication.

## Implementation Details

### Core Components

#### 1. PhantomDB Struct

The main client struct that provides methods for all PhantomDB operations:

- **BaseURL**: The base URL for the PhantomDB server
- **HTTPClient**: Configured HTTP client with timeout
- **Methods**: All database operations (CRUD, transactions, etc.)

#### 2. PhantomDBError Struct

Custom error type for handling PhantomDB-specific errors:

- **Message**: Error message string
- **Error()**: Implements the error interface

#### 3. Column Struct

Represents a table column definition:

- **Name**: Column name
- **Type**: Column type

### Key Features

#### HTTP Client Configuration

The client uses a configured HTTP client with appropriate timeouts:

```go
HTTPClient: &http.Client{
    Timeout: 30 * time.Second,
}
```

#### Error Handling

Comprehensive error handling with custom error types:

```go
if err != nil {
    if phantomErr, ok := err.(*phantomdb.PhantomDBError); ok {
        // Handle PhantomDB-specific errors
        fmt.Printf("PhantomDB error: %s\n", phantomErr.Error())
    } else {
        // Handle other errors
        fmt.Printf("Unexpected error: %s\n", err.Error())
    }
}
```

#### JSON Marshaling/Unmarshaling

Proper handling of JSON data for API communication:

```go
// Marshal request data
jsonData, err := json.Marshal(data)
if err != nil {
    return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to marshal data: %v", err)}
}

// Unmarshal response data
var result map[string]interface{}
if err := json.Unmarshal(body, &result); err != nil {
    return nil, &PhantomDBError{Message: fmt.Sprintf("Failed to unmarshal response: %v", err)}
}
```

### API Coverage

The Go client implements all the same functionality as the Python and JavaScript clients:

1. **Health Check**: Server status monitoring
2. **Database Operations**: Create, list, drop databases
3. **Table Operations**: Create, list, drop tables
4. **Data Operations**: Insert, select, update, delete records
5. **Query Operations**: Execute custom SQL queries
6. **Transaction Operations**: Begin, commit, rollback transactions

### Implementation Approach

The Go implementation follows Go idioms and best practices:

1. **Idiomatic Go**: Uses proper Go naming conventions and patterns
2. **Error Handling**: Proper error wrapping and handling
3. **HTTP Abstraction**: Internal `makeRequest` method handles all HTTP communication
4. **Response Processing**: Consistent response format handling
5. **Zero Dependencies**: Uses only standard library packages

## File Structure

```
src/clients/go/
├── go.mod              # Go module definition
├── phantomdb.go        # Main client implementation
├── phantomdb_test.go   # Unit tests
├── example.go          # Usage examples
└── README.md           # Documentation
```

## Usage Examples

### Basic Usage

```go
package main

import (
    "fmt"
    "log"
    
    "github.com/phantomdb/phantomdb-go"
)

func main() {
    // Connect to PhantomDB server
    db := phantomdb.New("http://localhost:8080")
    
    // Check server health
    health, err := db.HealthCheck()
    if err != nil {
        log.Fatal(err)
    }
    fmt.Printf("Server health: %+v\n", health)
    
    // Create a database
    result, err := db.CreateDatabase("example_db")
    if err != nil {
        log.Fatal(err)
    }
    fmt.Printf("Create database result: %+v\n", result)
}
```

### Advanced Usage

```go
// Create a table
columns := []phantomdb.Column{
    {Name: "id", Type: "int"},
    {Name: "name", Type: "string"},
    {Name: "email", Type: "string"},
}

result, err := db.CreateTable("example_db", "users", columns)
if err != nil {
    log.Fatal(err)
}
fmt.Printf("Create table result: %+v\n", result)

// Insert data
userData := map[string]interface{}{
    "id":    1,
    "name":  "John Doe",
    "email": "john@example.com",
}

insertResult, err := db.Insert("example_db", "users", userData)
if err != nil {
    log.Fatal(err)
}
fmt.Printf("Insert result: %+v\n", insertResult)

// Query data
users, err := db.Select("example_db", "users", nil)
if err != nil {
    log.Fatal(err)
}
fmt.Printf("Users: %+v\n", users)
```

## Testing

The Go client includes comprehensive tests:

1. **Unit Tests**: Tests for all methods using mock HTTP clients
2. **Integration Tests**: Tests using httptest server
3. **Error Handling Tests**: Tests for various error conditions

### Test Structure

```go
func TestHealthCheck(t *testing.T) {
    // Mock response
    response := map[string]interface{}{
        "status": "healthy",
    }
    jsonResponse, _ := json.Marshal(response)

    // Create mock client
    db := &PhantomDB{
        BaseURL: "http://localhost:8080",
        HTTPClient: &mockHTTPClient{
            responseBody: jsonResponse,
            statusCode:   200,
        },
    }

    // Test health check
    result, err := db.HealthCheck()
    if err != nil {
        t.Fatalf("HealthCheck failed: %v", err)
    }

    if result["status"] != "healthy" {
        t.Errorf("Expected status 'healthy', got %v", result["status"])
    }
}
```

## Dependencies

The Go client has no external dependencies, using only standard library packages:

- `net/http`: For HTTP communication
- `encoding/json`: For JSON marshaling/unmarshaling
- `time`: For timeout configuration
- `bytes`: For request body handling
- `io`: For response body handling
- `testing`: For unit tests
- `net/http/httptest`: For integration tests

## Future Enhancements

Planned improvements for the Go client:

1. **Connection Pooling**: Reuse HTTP connections for better performance
2. **Batch Operations**: Support for batch insert/update operations
3. **Query Builder**: Fluent API for building SQL queries
4. **Context Support**: Support for context cancellation and timeouts
5. **Authentication**: Support for authentication mechanisms

## Comparison with Other Clients

| Feature | Python Client | JavaScript Client | Go Client |
|---------|---------------|-------------------|-----------|
| HTTP Library | requests | fetch | net/http |
| Async Support | Yes | Yes (Promises) | Yes (goroutines) |
| Error Handling | Custom exceptions | Custom errors | Custom errors |
| Environment Support | Python | Node.js + Browser | Go |
| Package Management | pip | npm | go mod |
| Testing Framework | unittest | Jest | testing |

All clients provide the same functionality with similar APIs, making it easy for developers to switch between languages.