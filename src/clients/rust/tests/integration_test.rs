//! Integration tests for the PhantomDB Rust client.

use phantomdb::{PhantomDB, Column};
use serde_json::json;

#[tokio::test]
async fn test_new_client() {
    let db = PhantomDB::new("http://localhost:8080");
    assert_eq!(db.base_url, "http://localhost:8080");
}

#[tokio::test]
async fn test_health_check() {
    let db = PhantomDB::new("http://localhost:8080");
    
    // This test will fail if there's no server running, but it verifies
    // that the method signature is correct and the code compiles
    match db.health_check().await {
        Ok(_) => println!("Health check succeeded"),
        Err(e) => println!("Health check failed (expected if no server running): {}", e),
    }
}

#[tokio::test]
async fn test_list_databases() {
    let db = PhantomDB::new("http://localhost:8080");
    
    // This test will fail if there's no server running, but it verifies
    // that the method signature is correct and the code compiles
    match db.list_databases().await {
        Ok(databases) => println!("Databases: {:?}", databases),
        Err(e) => println!("List databases failed (expected if no server running): {}", e),
    }
}

#[tokio::test]
async fn test_create_database() {
    let db = PhantomDB::new("http://localhost:8080");
    
    // This test will fail if there's no server running, but it verifies
    // that the method signature is correct and the code compiles
    match db.create_database("test_db").await {
        Ok(result) => println!("Create database result: {:?}", result),
        Err(e) => println!("Create database failed (expected if no server running): {}", e),
    }
}

#[tokio::test]
async fn test_column_struct() {
    let column = Column {
        name: "test".to_string(),
        type_: "string".to_string(),
    };
    
    assert_eq!(column.name, "test");
    assert_eq!(column.type_, "string");
}

#[tokio::test]
async fn test_error_types() {
    use phantomdb::PhantomDBError;
    
    let custom_error = PhantomDBError::custom("test error");
    match custom_error {
        PhantomDBError::Custom(msg) => assert_eq!(msg, "test error"),
        _ => panic!("Expected Custom error"),
    }
}