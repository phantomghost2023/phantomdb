# PhantomDB Go Client

A Go client library for interacting with PhantomDB through its REST API.

## Installation

```bash
go get github.com/phantomdb/phantomdb-go
```

## Usage

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
    
    // List databases
    databases, err := db.ListDatabases()
    if err != nil {
        log.Fatal(err)
    }
    fmt.Printf("Databases: %+v\n", databases)
}
```

### Database Operations

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

// Update data
updateData := map[string]interface{}{
    "name": "Jane Doe",
}

condition := "id = 1"
updateResult, err := db.Update("example_db", "users", updateData, &condition)
if err != nil {
    log.Fatal(err)
}
fmt.Printf("Update result: %+v\n", updateResult)

// Delete data
deleteResult, err := db.Delete("example_db", "users", &condition)
if err != nil {
    log.Fatal(err)
}
fmt.Printf("Delete result: %+v\n", deleteResult)
```

### Transaction Operations

```go
// Begin a transaction
txnID, err := db.BeginTransaction()
if err != nil {
    log.Fatal(err)
}
fmt.Printf("Transaction ID: %s\n", txnID)

// Perform operations within transaction
userData := map[string]interface{}{
    "id":    2,
    "name":  "Bob Johnson",
    "email": "bob@example.com",
}

_, err = db.Insert("example_db", "users", userData)
if err != nil {
    // Rollback on error
    rollbackResult, rollbackErr := db.RollbackTransaction(txnID)
    if rollbackErr != nil {
        log.Fatal(rollbackErr)
    }
    fmt.Printf("Rollback result: %+v\n", rollbackResult)
    log.Fatal(err)
}

// Commit the transaction
commitResult, err := db.CommitTransaction(txnID)
if err != nil {
    log.Fatal(err)
}
fmt.Printf("Commit result: %+v\n", commitResult)
```

## API Reference

### Constructor

```go
db := phantomdb.New(baseURL)
```

- `baseURL` (string): The base URL of the PhantomDB REST API server. Defaults to `http://localhost:8080` if empty.

### Methods

#### Health Check
```go
health, err := db.HealthCheck()
```

#### Database Operations
- `ListDatabases() ([]string, error)`: List all databases
- `CreateDatabase(name string) (map[string]interface{}, error)`: Create a new database
- `DropDatabase(name string) (map[string]interface{}, error)`: Drop a database

#### Table Operations
- `ListTables(database string) ([]string, error)`: List all tables in a database
- `CreateTable(database, name string, columns []Column) (map[string]interface{}, error)`: Create a new table
- `DropTable(database, name string) (map[string]interface{}, error)`: Drop a table

#### Data Operations
- `Insert(database, table string, data map[string]interface{}) (map[string]interface{}, error)`: Insert data into a table
- `Select(database, table string, condition *string) ([]map[string]interface{}, error)`: Select data from a table
- `Update(database, table string, data map[string]interface{}, condition *string) (map[string]interface{}, error)`: Update data in a table
- `Delete(database, table string, condition *string) (map[string]interface{}, error)`: Delete data from a table

#### Query Operations
- `ExecuteQuery(database, query string) (map[string]interface{}, error)`: Execute a custom SQL query

#### Transaction Operations
- `BeginTransaction() (string, error)`: Begin a new transaction
- `CommitTransaction(transactionID string) (map[string]interface{}, error)`: Commit a transaction
- `RollbackTransaction(transactionID string) (map[string]interface{}, error)`: Rollback a transaction

## Error Handling

All methods can return a `PhantomDBError` if the request fails:

```go
result, err := db.CreateDatabase("test")
if err != nil {
    if phantomErr, ok := err.(*phantomdb.PhantomDBError); ok {
        fmt.Printf("PhantomDB error: %s\n", phantomErr.Error())
    } else {
        fmt.Printf("Unexpected error: %s\n", err.Error())
    }
}
```

## Requirements

- Go 1.19 or higher

## License

MIT