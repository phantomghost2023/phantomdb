# PhantomDB Rust Client

A Rust client library for interacting with PhantomDB through its REST API.

## Installation

Add this to your `Cargo.toml`:

```toml
[dependencies]
phantomdb = { path = "../path/to/phantomdb/rust" }
tokio = { version = "1.0", features = ["full"] }
```

## Usage

### Basic Usage

```rust
use phantomdb::PhantomDB;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    // Connect to PhantomDB server
    let db = PhantomDB::new("http://localhost:8080");
    
    // Check server health
    let health = db.health_check().await?;
    println!("Server health: {:?}", health);
    
    // Create a database
    let result = db.create_database("example_db").await?;
    println!("Create database result: {:?}", result);
    
    // List databases
    let databases = db.list_databases().await?;
    println!("Databases: {:?}", databases);
    
    Ok(())
}
```

### Database Operations

```rust
use phantomdb::{PhantomDB, Column};

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let db = PhantomDB::new("http://localhost:8080");
    
    // Create a table
    let columns = vec![
        Column {
            name: "id".to_string(),
            type_: "int".to_string(),
        },
        Column {
            name: "name".to_string(),
            type_: "string".to_string(),
        },
        Column {
            name: "email".to_string(),
            type_: "string".to_string(),
        },
    ];
    
    let result = db.create_table("example_db", "users", columns).await?;
    println!("Create table result: {:?}", result);
    
    // Insert data
    let user_data = serde_json::json!({
        "id": 1,
        "name": "John Doe",
        "email": "john@example.com"
    });
    
    let insert_result = db.insert("example_db", "users", user_data).await?;
    println!("Insert result: {:?}", insert_result);
    
    // Query data
    let users = db.select("example_db", "users", None).await?;
    println!("Users: {:?}", users);
    
    // Update data
    let update_data = serde_json::json!({
        "name": "Jane Doe"
    });
    
    let condition = Some("id = 1");
    let update_result = db.update("example_db", "users", update_data, condition).await?;
    println!("Update result: {:?}", update_result);
    
    // Delete data
    let delete_result = db.delete("example_db", "users", condition).await?;
    println!("Delete result: {:?}", delete_result);
    
    Ok(())
}
```

### Transaction Operations

```rust
use phantomdb::PhantomDB;
use serde_json::json;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let db = PhantomDB::new("http://localhost:8080");
    
    // Begin a transaction
    let txn_id = db.begin_transaction().await?;
    println!("Transaction ID: {}", txn_id);
    
    // Perform operations within transaction
    let user_data = json!({
        "id": 2,
        "name": "Bob Johnson",
        "email": "bob@example.com"
    });
    
    match db.insert("example_db", "users", user_data).await {
        Ok(_) => {
            // Commit the transaction
            let commit_result = db.commit_transaction(&txn_id).await?;
            println!("Commit result: {:?}", commit_result);
        }
        Err(e) => {
            // Rollback on error
            println!("Error occurred, rolling back: {}", e);
            let rollback_result = db.rollback_transaction(&txn_id).await?;
            println!("Rollback result: {:?}", rollback_result);
        }
    }
    
    Ok(())
}
```

## API Reference

### Constructor

```rust
let db = PhantomDB::new(base_url);
```

- `base_url` (&str): The base URL of the PhantomDB REST API server. Defaults to `http://localhost:8080` if empty.

### Methods

#### Health Check
```rust
let health = db.health_check().await?;
```

#### Database Operations
- `list_databases() -> Result<Vec<String>, PhantomDBError>`: List all databases
- `create_database(name: &str) -> Result<serde_json::Value, PhantomDBError>`: Create a new database
- `drop_database(name: &str) -> Result<serde_json::Value, PhantomDBError>`: Drop a database

#### Table Operations
- `list_tables(database: &str) -> Result<Vec<String>, PhantomDBError>`: List all tables in a database
- `create_table(database: &str, name: &str, columns: Vec<Column>) -> Result<serde_json::Value, PhantomDBError>`: Create a new table
- `drop_table(database: &str, name: &str) -> Result<serde_json::Value, PhantomDBError>`: Drop a table

#### Data Operations
- `insert(database: &str, table: &str, data: serde_json::Value) -> Result<serde_json::Value, PhantomDBError>`: Insert data into a table
- `select(database: &str, table: &str, condition: Option<&str>) -> Result<Vec<serde_json::Value>, PhantomDBError>`: Select data from a table
- `update(database: &str, table: &str, data: serde_json::Value, condition: Option<&str>) -> Result<serde_json::Value, PhantomDBError>`: Update data in a table
- `delete(database: &str, table: &str, condition: Option<&str>) -> Result<serde_json::Value, PhantomDBError>`: Delete data from a table

#### Query Operations
- `execute_query(database: &str, query: &str) -> Result<serde_json::Value, PhantomDBError>`: Execute a custom SQL query

#### Transaction Operations
- `begin_transaction() -> Result<String, PhantomDBError>`: Begin a new transaction
- `commit_transaction(transaction_id: &str) -> Result<serde_json::Value, PhantomDBError>`: Commit a transaction
- `rollback_transaction(transaction_id: &str) -> Result<serde_json::Value, PhantomDBError>`: Rollback a transaction

## Error Handling

All methods can return a `PhantomDBError` if the request fails:

```rust
match db.create_database("test").await {
    Ok(result) => println!("Success: {:?}", result),
    Err(PhantomDBError::HttpError(status, message)) => {
        println!("HTTP error {}: {}", status, message);
    }
    Err(PhantomDBError::JsonError(e)) => {
        println!("JSON error: {}", e);
    }
    Err(e) => {
        println!("Other error: {}", e);
    }
}
```

## Requirements

- Rust 1.56 or higher
- Tokio 1.0 or higher

## Features

- **Asynchronous**: Built with async/await for high performance
- **Strong Typing**: Uses Serde for JSON serialization/deserialization
- **Error Handling**: Comprehensive error types with `thiserror`
- **Zero-Copy**: Efficient HTTP client with `reqwest`
- **No Unsafe Code**: 100% safe Rust

## License

MIT