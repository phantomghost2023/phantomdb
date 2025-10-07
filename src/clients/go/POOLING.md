# PhantomDB Go Connection Pooling

This document provides documentation for the connection pooling functionality in the PhantomDB Go client.

## Overview

The PhantomDB Go connection pool provides a way to manage and reuse connections to the PhantomDB server, improving performance by reducing the overhead of creating new connections for each operation.

## Features

- Connection reuse to reduce overhead
- Configurable pool size
- Automatic connection validation
- Timeout management
- Thread-safe operations

## Installation

The connection pooling functionality is included in the main PhantomDB Go client package.

## Usage

### Creating a Connection Pool

```go
package main

import (
    "time"
    "phantomdb-go/pool"
)

func main() {
    // Create a connection pool with default settings
    config := pool.Config{
        BaseURL: "http://localhost:8080",
    }
    pool := pool.New(config)

    // Create a connection pool with custom settings
    customConfig := pool.Config{
        BaseURL:        "http://localhost:8080",
        MinConnections: 5,
        MaxConnections: 20,
        AcquireTimeout: 60 * time.Second,
        IdleTimeout:    5 * time.Minute,
    }
    customPool := pool.New(customConfig)
}
```

### Acquiring and Releasing Connections

```go
func performDatabaseOperations(p *pool.PhantomDBPool) {
    // Acquire a connection from the pool
    client, err := p.Acquire()
    if err != nil {
        log.Fatal("Failed to acquire connection:", err)
    }
    defer p.Release(client) // Always release the connection back to the pool

    // Use the client for database operations
    _, err = client.CreateDatabase("myapp")
    if err != nil {
        log.Fatal("Failed to create database:", err)
    }
    
    databases, err := client.ListDatabases()
    if err != nil {
        log.Fatal("Failed to list databases:", err)
    }
    fmt.Println("Databases:", databases)
    
    // Perform other operations...
}
```

### Pool Status Monitoring

```go
func monitorPoolStatus(p *pool.PhantomDBPool) {
    // Get current pool status
    status := p.Status()
    fmt.Printf("Pool status: %+v\n", status)
    // Output: Pool status: map[available:2 max:10 min:2 used:0]
}
```

## Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `BaseURL` | `"http://localhost:8080"` | The base URL of the PhantomDB REST API server |
| `MinConnections` | `2` | Minimum number of connections in the pool |
| `MaxConnections` | `10` | Maximum number of connections in the pool |
| `AcquireTimeout` | `30 * time.Second` | Timeout for acquiring a connection |
| `IdleTimeout` | `60 * time.Second` | Timeout for idle connections |

## Error Handling

The connection pool provides proper error handling for various scenarios:

```go
func handlePoolErrors(p *pool.PhantomDBPool) {
    client, err := p.Acquire()
    if err != nil {
        if strings.Contains(err.Error(), "Timeout") {
            log.Println("Unable to acquire connection within timeout")
        } else if strings.Contains(err.Error(), "closed") {
            log.Println("Connection pool is closed")
        } else {
            log.Println("Connection error:", err)
        }
        return
    }
    defer p.Release(client)
    
    // ... use client
}
```

## Best Practices

1. **Always release connections**: Make sure to release connections back to the pool after use
2. **Use defer statements**: Use defer to ensure connections are released even if an error occurs
3. **Close the pool**: Close the pool when your application shuts down
4. **Monitor pool status**: Use `Status()` to monitor pool performance
5. **Configure appropriately**: Set pool sizes based on your application's needs

## API Reference

### `pool.Config`

Configuration struct for connection pool.

**Fields:**
- `BaseURL` (string): The base URL of the PhantomDB REST API server
- `MinConnections` (int): Minimum number of connections in the pool
- `MaxConnections` (int): Maximum number of connections in the pool
- `AcquireTimeout` (time.Duration): Timeout for acquiring a connection
- `IdleTimeout` (time.Duration): Timeout for idle connections

### `pool.New(config)`

Creates a new connection pool.

**Parameters:**
- `config` (pool.Config): Configuration options

**Returns:**
- `*PhantomDBPool`: A new connection pool instance

### `PhantomDBPool`

#### `Acquire()`

Acquires a connection from the pool.

**Returns:**
- `*phantomdb.PhantomDB`: A PhantomDB client instance
- `error`: Error if unable to acquire a connection

#### `Release(connection)`

Releases a connection back to the pool.

**Parameters:**
- `connection` (*phantomdb.PhantomDB): The PhantomDB client instance to release

#### `Close()`

Closes the connection pool and all connections.

#### `Status()`

Gets the current pool status.

**Returns:**
- `map[string]int`: Pool status information

## Examples

### Web Server Integration

```go
package main

import (
    "encoding/json"
    "log"
    "net/http"
    "phantomdb-go/pool"
)

var dbPool *pool.PhantomDBPool

func init() {
    config := pool.Config{
        BaseURL: "http://localhost:8080",
    }
    dbPool = pool.New(config)
}

func listDatabasesHandler(w http.ResponseWriter, r *http.Request) {
    client, err := dbPool.Acquire()
    if err != nil {
        http.Error(w, err.Error(), http.StatusInternalServerError)
        return
    }
    defer dbPool.Release(client)

    databases, err := client.ListDatabases()
    if err != nil {
        http.Error(w, err.Error(), http.StatusInternalServerError)
        return
    }

    w.Header().Set("Content-Type", "application/json")
    json.NewEncoder(w).Encode(map[string]interface{}{
        "databases": databases,
    })
}

func main() {
    http.HandleFunc("/databases", listDatabasesHandler)
    
    log.Println("Server starting on :8080")
    log.Fatal(http.ListenAndServe(":8080", nil))
}
```

### Concurrent Application

```go
package main

import (
    "fmt"
    "sync"
    "phantomdb-go/pool"
)

func worker(p *pool.PhantomDBPool, workerID int, wg *sync.WaitGroup) {
    defer wg.Done()
    
    for i := 0; i < 5; i++ {
        client, err := p.Acquire()
        if err != nil {
            fmt.Printf("Worker %d, iteration %d: Failed to acquire connection: %v\n", workerID, i, err)
            continue
        }
        
        databases, err := client.ListDatabases()
        if err != nil {
            fmt.Printf("Worker %d, iteration %d: Failed to list databases: %v\n", workerID, i, err)
            p.Release(client)
            continue
        }
        
        fmt.Printf("Worker %d, iteration %d: Found %d databases\n", workerID, i, len(databases))
        p.Release(client)
    }
}

func runConcurrentExample() {
    config := pool.Config{
        BaseURL: "http://localhost:8080",
    }
    pool := pool.New(config)
    
    var wg sync.WaitGroup
    for i := 0; i < 3; i++ {
        wg.Add(1)
        go worker(pool, i, &wg)
    }
    
    wg.Wait()
    
    // Check final pool status
    status := pool.Status()
    fmt.Printf("Final pool status: %+v\n", status)
    
    pool.Close()
}
```

## Testing

The connection pool includes comprehensive tests:

```go
// pool/pool_test.go
package pool

import (
    "testing"
    "time"
)

func TestCreatePoolWithDefaultSettings(t *testing.T) {
    config := Config{
        BaseURL: "http://localhost:8080",
    }
    pool := New(config)
    
    status := pool.Status()
    if status["min"] != 2 {
        t.Errorf("Expected min connections to be 2, got %d", status["min"])
    }
    if status["max"] != 10 {
        t.Errorf("Expected max connections to be 10, got %d", status["max"])
    }
}

func TestAcquireAndReleaseConnections(t *testing.T) {
    config := Config{
        BaseURL: "http://localhost:8080",
    }
    pool := New(config)
    
    client, err := pool.Acquire()
    if err != nil {
        t.Fatalf("Failed to acquire connection: %v", err)
    }
    
    pool.Release(client)
}
```

## License

MIT License