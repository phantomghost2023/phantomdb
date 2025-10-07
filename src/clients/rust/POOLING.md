# PhantomDB Rust Connection Pooling

This document provides documentation for the connection pooling functionality in the PhantomDB Rust client.

## Overview

The PhantomDB Rust connection pool provides a way to manage and reuse connections to the PhantomDB server, improving performance by reducing the overhead of creating new connections for each operation. The pool is built on top of Tokio's async runtime and uses semaphore-based concurrency control.

## Features

- Async/await support with Tokio
- Connection reuse to reduce overhead
- Configurable pool size
- Automatic connection validation
- Timeout management
- Thread-safe operations
- Automatic connection release using RAII (Resource Acquisition Is Initialization)

## Installation

The connection pooling functionality is included in the main PhantomDB Rust client package.

Add this to your `Cargo.toml`:

```toml
[dependencies]
phantomdb = "0.1"
tokio = { version = "1.0", features = ["full"] }
```

## Usage

### Creating a Connection Pool

```rust
use phantomdb::pool::{PhantomDBPool, PoolConfig, create_pool, create_pool_with_config};

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    // Create a connection pool with default settings
    let pool = create_pool();
    
    // Create a connection pool with custom settings
    let config = PoolConfig {
        base_url: "http://localhost:8080".to_string(),
        min_connections: 5,
        max_connections: 20,
        acquire_timeout: std::time::Duration::from_secs(60),
        idle_timeout: std::time::Duration::from_secs(300),
    };
    let custom_pool = create_pool_with_config(config);
    
    Ok(())
}
```

### Acquiring and Using Connections

```rust
use phantomdb::pool::create_pool;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let pool = create_pool();
    
    // Acquire a connection from the pool
    let mut pooled_connection = pool.acquire().await?;
    
    // Use the client for database operations
    let result = pooled_connection.inner().create_database("myapp").await?;
    println!("Create database result: {:?}", result);
    
    let databases = pooled_connection.inner().list_databases().await?;
    println!("Databases: {:?}", databases);
    
    // Connection is automatically released when `pooled_connection` goes out of scope
    Ok(())
}
```

### Pool Status Monitoring

```rust
use phantomdb::pool::create_pool;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let pool = create_pool();
    
    // Get current pool status
    let status = pool.status();
    println!("Pool status: {:?}", status);
    // Output: Pool status: PoolStatus { available: 2, used: 0, min: 2, max: 10 }
    
    Ok(())
}
```

## Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `base_url` | `"http://localhost:8080"` | The base URL of the PhantomDB REST API server |
| `min_connections` | `2` | Minimum number of connections in the pool |
| `max_connections` | `10` | Maximum number of connections in the pool |
| `acquire_timeout` | `30 seconds` | Timeout for acquiring a connection |
| `idle_timeout` | `60 seconds` | Timeout for idle connections |

## Error Handling

The connection pool provides proper error handling for various scenarios:

```rust
use phantomdb::pool::create_pool;
use phantomdb::PhantomDBError;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let pool = create_pool();
    
    match pool.acquire().await {
        Ok(mut pooled_connection) => {
            // Use the connection
            match pooled_connection.inner().list_databases().await {
                Ok(databases) => println!("Databases: {:?}", databases),
                Err(PhantomDBError::ConnectionError(msg)) => {
                    eprintln!("Connection error: {}", msg);
                }
                Err(PhantomDBError::HttpError(status, msg)) => {
                    eprintln!("HTTP error {}: {}", status, msg);
                }
                Err(e) => eprintln!("Other error: {}", e),
            }
            // Connection is automatically released here
        }
        Err(PhantomDBError::ConnectionError(msg)) if msg.contains("Timeout") => {
            eprintln!("Unable to acquire connection within timeout");
        }
        Err(PhantomDBError::ConnectionError(msg)) if msg.contains("closed") => {
            eprintln!("Connection pool is closed");
        }
        Err(e) => {
            eprintln!("Connection error: {}", e);
        }
    }
    
    Ok(())
}
```

## Best Practices

1. **Use RAII**: The `PooledConnection` automatically releases the connection when it goes out of scope
2. **Clone the pool**: Clone the pool instance to share it across tasks
3. **Close the pool**: Close the pool when your application shuts down
4. **Monitor pool status**: Use `status()` to monitor pool performance
5. **Configure appropriately**: Set pool sizes based on your application's needs
6. **Handle timeouts**: Set appropriate timeouts for your use case

## API Reference

### `PoolConfig`

Configuration struct for connection pool.

**Fields:**
- `base_url` (String): The base URL of the PhantomDB REST API server
- `min_connections` (usize): Minimum number of connections in the pool
- `max_connections` (usize): Maximum number of connections in the pool
- `acquire_timeout` (Duration): Timeout for acquiring a connection
- `idle_timeout` (Duration): Timeout for idle connections

### `create_pool()`

Creates a new connection pool with default configuration.

**Returns:**
- `PhantomDBPool`: A new connection pool instance

### `create_pool_with_config(config)`

Creates a new connection pool with custom configuration.

**Parameters:**
- `config` (PoolConfig): Configuration options

**Returns:**
- `PhantomDBPool`: A new connection pool instance

### `PhantomDBPool`

#### `acquire()`

Acquires a connection from the pool.

**Returns:**
- `Result<PooledConnection, PhantomDBError>`: A pooled connection wrapper

#### `close()`

Closes the connection pool and all connections.

#### `status()`

Gets the current pool status.

**Returns:**
- `PoolStatus`: Pool status information

### `PooledConnection`

A wrapper around a PhantomDB client that automatically returns the connection to the pool when dropped.

#### `inner()`

Gets a reference to the underlying PhantomDB client.

**Returns:**
- `&PhantomDB`: Reference to the PhantomDB client

#### `inner_mut()`

Gets a mutable reference to the underlying PhantomDB client.

**Returns:**
- `&mut PhantomDB`: Mutable reference to the PhantomDB client

### `PoolStatus`

Status information for a connection pool.

**Fields:**
- `available` (usize): Number of available connections
- `used` (usize): Number of used connections
- `min` (usize): Minimum number of connections
- `max` (usize): Maximum number of connections

## Examples

### Web Server Integration

```rust
use axum::{
    extract::Extension,
    routing::get,
    Json, Router, Server,
};
use phantomdb::pool::PhantomDBPool;
use serde_json::Value;
use std::net::SocketAddr;

#[tokio::main]
async fn main() {
    let pool = PhantomDBPool::new(Default::default());
    
    let app = Router::new()
        .route("/databases", get(list_databases))
        .layer(Extension(pool));
    
    let addr = SocketAddr::from(([127, 0, 0, 1], 3000));
    Server::bind(&addr)
        .serve(app.into_make_service())
        .await
        .unwrap();
}

async fn list_databases(
    Extension(pool): Extension<PhantomDBPool>
) -> Result<Json<Value>, (axum::http::StatusCode, String)> {
    let pooled_connection = pool.acquire().await
        .map_err(|e| (axum::http::StatusCode::INTERNAL_SERVER_ERROR, e.to_string()))?;
    
    let databases = pooled_connection.inner().list_databases().await
        .map_err(|e| (axum::http::StatusCode::INTERNAL_SERVER_ERROR, e.to_string()))?;
    
    Ok(Json(serde_json::json!({ "databases": databases })))
}
```

### Concurrent Application

```rust
use phantomdb::pool::create_pool;
use tokio::time::{sleep, Duration};

async fn worker(pool: PhantomDBPool, worker_id: usize) {
    for i in 0..5 {
        match pool.acquire().await {
            Ok(mut pooled_connection) => {
                match pooled_connection.inner().list_databases().await {
                    Ok(databases) => {
                        println!("Worker {}, iteration {}: Found {} databases", 
                                worker_id, i, databases.len());
                    }
                    Err(e) => {
                        eprintln!("Worker {}, iteration {}: Failed to list databases: {}", 
                                 worker_id, i, e);
                    }
                }
                // Connection is automatically released here
            }
            Err(e) => {
                eprintln!("Worker {}, iteration {}: Failed to acquire connection: {}", 
                         worker_id, i, e);
            }
        }
        
        // Simulate work
        sleep(Duration::from_millis(100)).await;
    }
}

#[tokio::main]
async fn run_concurrent_example() -> Result<(), Box<dyn std::error::Error>> {
    let pool = create_pool();
    
    // Spawn multiple worker tasks
    let mut handles = vec![];
    for i in 0..3 {
        let pool_clone = pool.clone();
        let handle = tokio::spawn(async move {
            worker(pool_clone, i).await;
        });
        handles.push(handle);
    }
    
    // Wait for all tasks to complete
    for handle in handles {
        handle.await?;
    }
    
    // Check final pool status
    let status = pool.status();
    println!("Final pool status: {:?}", status);
    
    pool.close();
    Ok(())
}
```

## Testing

The connection pool includes comprehensive tests:

```rust
// src/pool.rs
#[cfg(test)]
mod tests {
    use super::*;
    
    #[tokio::test]
    async fn test_create_pool_with_default_settings() {
        let pool = create_pool();
        let status = pool.status();
        assert_eq!(status.min, 2);
        assert_eq!(status.max, 10);
    }
    
    #[tokio::test]
    async fn test_acquire_and_release_connections() {
        let pool = create_pool();
        let pooled_connection = pool.acquire().await;
        assert!(pooled_connection.is_ok());
        // Connection is automatically released when `pooled_connection` goes out of scope
    }
    
    #[tokio::test]
    async fn test_pool_status() {
        let pool = create_pool();
        let initial_status = pool.status();
        assert_eq!(initial_status.available, 2);
        assert_eq!(initial_status.used, 0);
    }
}
```

## License

MIT License