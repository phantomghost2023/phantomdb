//! Example usage of the PhantomDB Rust client.

use phantomdb::{PhantomDB, Column};
use serde_json::json;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    // Connect to PhantomDB server
    let db = PhantomDB::new("http://localhost:8080");

    // Check server health
    println!("Checking server health...");
    match db.health_check().await {
        Ok(health) => println!("Server health: {:?}", health),
        Err(e) => println!("Health check failed: {}", e),
    }

    // Create a database
    println!("\nCreating database 'example_db'...");
    match db.create_database("example_db").await {
        Ok(result) => println!("Create database result: {:?}", result),
        Err(e) => println!("Create database failed: {}", e),
    }

    // List databases
    println!("\nListing databases...");
    match db.list_databases().await {
        Ok(databases) => println!("Databases: {:?}", databases),
        Err(e) => println!("List databases failed: {}", e),
    }

    // Create a table
    println!("\nCreating table 'users'...");
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

    match db.create_table("example_db", "users", columns).await {
        Ok(result) => println!("Create table result: {:?}", result),
        Err(e) => println!("Create table failed: {}", e),
    }

    // List tables
    println!("\nListing tables...");
    match db.list_tables("example_db").await {
        Ok(tables) => println!("Tables: {:?}", tables),
        Err(e) => println!("List tables failed: {}", e),
    }

    // Insert data
    println!("\nInserting data...");
    let user_data = json!({
        "id": 1,
        "name": "John Doe",
        "email": "john@example.com"
    });

    match db.insert("example_db", "users", user_data).await {
        Ok(result) => println!("Insert result: {:?}", result),
        Err(e) => println!("Insert failed: {}", e),
    }

    // Insert another record
    let user_data2 = json!({
        "id": 2,
        "name": "Jane Smith",
        "email": "jane@example.com"
    });

    match db.insert("example_db", "users", user_data2).await {
        Ok(result) => println!("Insert result: {:?}", result),
        Err(e) => println!("Insert failed: {}", e),
    }

    // Query data
    println!("\nQuerying data...");
    match db.select("example_db", "users", None).await {
        Ok(users) => println!("Users: {:?}", users),
        Err(e) => println!("Select failed: {}", e),
    }

    // Update data
    println!("\nUpdating data...");
    let update_data = json!({
        "name": "Jane Doe"
    });

    let condition = Some("id = 2");
    match db.update("example_db", "users", update_data, condition).await {
        Ok(result) => println!("Update result: {:?}", result),
        Err(e) => println!("Update failed: {}", e),
    }

    // Query updated data
    println!("\nQuerying updated data...");
    match db.select("example_db", "users", None).await {
        Ok(users) => println!("Users: {:?}", users),
        Err(e) => println!("Select failed: {}", e),
    }

    // Delete data
    println!("\nDeleting data...");
    match db.delete("example_db", "users", condition).await {
        Ok(result) => println!("Delete result: {:?}", result),
        Err(e) => println!("Delete failed: {}", e),
    }

    // Query remaining data
    println!("\nQuerying remaining data...");
    match db.select("example_db", "users", None).await {
        Ok(users) => println!("Users: {:?}", users),
        Err(e) => println!("Select failed: {}", e),
    }

    // Demonstrate transactions
    println!("\nDemonstrating transactions...");
    match db.begin_transaction().await {
        Ok(txn_id) => {
            println!("Transaction ID: {}", txn_id);

            // Insert data within transaction
            let user_data3 = json!({
                "id": 3,
                "name": "Bob Johnson",
                "email": "bob@example.com"
            });

            match db.insert("example_db", "users", user_data3).await {
                Ok(_) => {
                    // Commit the transaction
                    match db.commit_transaction(&txn_id).await {
                        Ok(result) => println!("Commit result: {:?}", result),
                        Err(e) => println!("Commit failed: {}", e),
                    }
                }
                Err(e) => {
                    // Rollback on error
                    println!("Error occurred, rolling back: {}", e);
                    match db.rollback_transaction(&txn_id).await {
                        Ok(result) => println!("Rollback result: {:?}", result),
                        Err(e) => println!("Rollback failed: {}", e),
                    }
                }
            }
        }
        Err(e) => println!("Begin transaction failed: {}", e),
    }

    // Final query
    println!("\nFinal data query...");
    match db.select("example_db", "users", None).await {
        Ok(users) => println!("Users: {:?}", users),
        Err(e) => println!("Select failed: {}", e),
    }

    // Drop table
    println!("\nDropping table 'users'...");
    match db.drop_table("example_db", "users").await {
        Ok(result) => println!("Drop table result: {:?}", result),
        Err(e) => println!("Drop table failed: {}", e),
    }

    // Drop database
    println!("\nDropping database 'example_db'...");
    match db.drop_database("example_db").await {
        Ok(result) => println!("Drop database result: {:?}", result),
        Err(e) => println!("Drop database failed: {}", e),
    }

    Ok(())
}