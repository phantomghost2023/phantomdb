//! # PhantomDB Rust Client
//!
//! A Rust client library for interacting with PhantomDB through its REST API.
//!
//! ## Features
//!
//! - Asynchronous API using Tokio
//! - Strong typing with Serde
//! - Comprehensive error handling
//! - Full coverage of PhantomDB REST API
//! - Connection pooling support
//!
//! ## Usage
//!
//! ```rust
//! use phantomdb::PhantomDB;
//!
//! #[tokio::main]
//! async fn main() -> Result<(), Box<dyn std::error::Error>> {
//!     let db = PhantomDB::new("http://localhost:8080");
//!     
//!     // Check server health
//!     let health = db.health_check().await?;
//!     println!("Server health: {:?}", health);
//!     
//!     // Create a database
//!     let result = db.create_database("example_db").await?;
//!     println!("Create database result: {:?}", result);
//!     
//!     Ok(())
//! }
//! ```

mod error;
mod types;
mod pool;

pub use error::PhantomDBError;
pub use types::{Column, DatabaseInfo, TableInfo};
pub use pool::{PhantomDBPool, PoolConfig, PooledConnection, PoolStatus, create_pool, create_pool_with_config};

use reqwest::Client;
use serde::{Deserialize, Serialize};
use std::time::Duration;

/// PhantomDB client for interacting with the PhantomDB REST API.
pub struct PhantomDB {
    base_url: String,
    client: Client,
}

impl PhantomDB {
    /// Create a new PhantomDB client.
    ///
    /// # Arguments
    ///
    /// * `base_url` - The base URL of the PhantomDB REST API server
    ///
    /// # Returns
    ///
    /// A new PhantomDB client instance
    ///
    /// # Examples
    ///
    /// ```
    /// use phantomdb::PhantomDB;
    ///
    /// let db = PhantomDB::new("http://localhost:8080");
    /// ```
    pub fn new(base_url: &str) -> Self {
        let client = Client::builder()
            .timeout(Duration::from_secs(30))
            .build()
            .expect("Failed to create HTTP client");
            
        Self {
            base_url: base_url.to_string(),
            client,
        }
    }

    /// Make an HTTP request to the PhantomDB REST API.
    async fn make_request<T, R>(
        &self,
        method: reqwest::Method,
        endpoint: &str,
        data: Option<T>,
    ) -> Result<R, PhantomDBError>
    where
        T: Serialize,
        R: for<'de> Deserialize<'de>,
    {
        let url = format!("{}{}", self.base_url, endpoint);
        
        let request = match method {
            reqwest::Method::GET | reqwest::Method::DELETE => {
                self.client.request(method, &url)
            }
            _ => {
                if let Some(body) = data {
                    self.client
                        .request(method, &url)
                        .json(&body)
                } else {
                    self.client.request(method, &url)
                }
            }
        };

        let response = request.send().await?;
        
        if !response.status().is_success() {
            return Err(PhantomDBError::HttpError(
                response.status(),
                response.status().to_string(),
            ));
        }
        
        let result = response.json::<R>().await?;
        Ok(result)
    }

    /// Check the health of the PhantomDB server.
    ///
    /// # Returns
    ///
    /// Health status information
    ///
    /// # Examples
    ///
    /// ```
    /// # use phantomdb::PhantomDB;
    /// # #[tokio::main]
    /// # async fn main() -> Result<(), Box<dyn std::error::Error>> {
    /// # let db = PhantomDB::new("http://localhost:8080");
    /// let health = db.health_check().await?;
    /// # Ok(())
    /// # }
    /// ```
    pub async fn health_check(&self) -> Result<serde_json::Value, PhantomDBError> {
        self.make_request(reqwest::Method::GET, "/health", None::<()>)
            .await
    }

    /// List all databases in PhantomDB.
    ///
    /// # Returns
    ///
    /// List of database names
    pub async fn list_databases(&self) -> Result<Vec<String>, PhantomDBError> {
        #[derive(Deserialize)]
        struct DatabaseList {
            databases: Vec<String>,
        }
        
        let result: DatabaseList = self
            .make_request(reqwest::Method::GET, "/databases", None::<()>)
            .await?;
        Ok(result.databases)
    }

    /// Create a new database.
    ///
    /// # Arguments
    ///
    /// * `name` - Name of the database to create
    ///
    /// # Returns
    ///
    /// Response from the server
    pub async fn create_database(&self, name: &str) -> Result<serde_json::Value, PhantomDBError> {
        #[derive(Serialize)]
        struct CreateDatabaseRequest {
            name: String,
        }
        
        let request = CreateDatabaseRequest {
            name: name.to_string(),
        };
        
        self.make_request(reqwest::Method::POST, "/databases", Some(request))
            .await
    }

    /// Drop a database.
    ///
    /// # Arguments
    ///
    /// * `name` - Name of the database to drop
    ///
    /// # Returns
    ///
    /// Response from the server
    pub async fn drop_database(&self, name: &str) -> Result<serde_json::Value, PhantomDBError> {
        let endpoint = format!("/databases/{}", name);
        self.make_request(reqwest::Method::DELETE, &endpoint, None::<()>)
            .await
    }

    /// List all tables in a database.
    ///
    /// # Arguments
    ///
    /// * `database` - Name of the database
    ///
    /// # Returns
    ///
    /// List of table names
    pub async fn list_tables(&self, database: &str) -> Result<Vec<String>, PhantomDBError> {
        #[derive(Deserialize)]
        struct TableList {
            tables: Vec<String>,
        }
        
        let endpoint = format!("/databases/{}/tables", database);
        let result: TableList = self
            .make_request(reqwest::Method::GET, &endpoint, None::<()>)
            .await?;
        Ok(result.tables)
    }

    /// Create a new table in a database.
    ///
    /// # Arguments
    ///
    /// * `database` - Name of the database
    /// * `name` - Name of the table to create
    /// * `columns` - List of column definitions
    ///
    /// # Returns
    ///
    /// Response from the server
    pub async fn create_table(
        &self,
        database: &str,
        name: &str,
        columns: Vec<Column>,
    ) -> Result<serde_json::Value, PhantomDBError> {
        #[derive(Serialize)]
        struct CreateTableRequest {
            name: String,
            columns: Vec<Column>,
        }
        
        let request = CreateTableRequest {
            name: name.to_string(),
            columns,
        };
        
        let endpoint = format!("/databases/{}/tables", database);
        self.make_request(reqwest::Method::POST, &endpoint, Some(request))
            .await
    }

    /// Drop a table from a database.
    ///
    /// # Arguments
    ///
    /// * `database` - Name of the database
    /// * `name` - Name of the table to drop
    ///
    /// # Returns
    ///
    /// Response from the server
    pub async fn drop_table(&self, database: &str, name: &str) -> Result<serde_json::Value, PhantomDBError> {
        let endpoint = format!("/databases/{}/tables/{}", database, name);
        self.make_request(reqwest::Method::DELETE, &endpoint, None::<()>)
            .await
    }

    /// Insert data into a table.
    ///
    /// # Arguments
    ///
    /// * `database` - Name of the database
    /// * `table` - Name of the table
    /// * `data` - Data to insert
    ///
    /// # Returns
    ///
    /// Response from the server
    pub async fn insert(
        &self,
        database: &str,
        table: &str,
        data: serde_json::Value,
    ) -> Result<serde_json::Value, PhantomDBError> {
        let endpoint = format!("/databases/{}/tables/{}", database, table);
        self.make_request(reqwest::Method::POST, &endpoint, Some(data))
            .await
    }

    /// Select data from a table.
    ///
    /// # Arguments
    ///
    /// * `database` - Name of the database
    /// * `table` - Name of the table
    /// * `condition` - Condition for filtering data (optional)
    ///
    /// # Returns
    ///
    /// List of records matching the query
    pub async fn select(
        &self,
        database: &str,
        table: &str,
        condition: Option<&str>,
    ) -> Result<Vec<serde_json::Value>, PhantomDBError> {
        let mut endpoint = format!("/databases/{}/tables/{}", database, table);
        if let Some(condition) = condition {
            endpoint.push_str(&format!("?condition={}", condition));
        }
        
        #[derive(Deserialize)]
        struct SelectResponse {
            records: Vec<serde_json::Value>,
        }
        
        let result: SelectResponse = self
            .make_request(reqwest::Method::GET, &endpoint, None::<()>)
            .await?;
        Ok(result.records)
    }

    /// Update data in a table.
    ///
    /// # Arguments
    ///
    /// * `database` - Name of the database
    /// * `table` - Name of the table
    /// * `data` - Data to update
    /// * `condition` - Condition for filtering data to update (optional)
    ///
    /// # Returns
    ///
    /// Response from the server
    pub async fn update(
        &self,
        database: &str,
        table: &str,
        data: serde_json::Value,
        condition: Option<&str>,
    ) -> Result<serde_json::Value, PhantomDBError> {
        #[derive(Serialize)]
        struct UpdateRequest {
            data: serde_json::Value,
            #[serde(skip_serializing_if = "Option::is_none")]
            condition: Option<String>,
        }
        
        let request = UpdateRequest {
            data,
            condition: condition.map(|s| s.to_string()),
        };
        
        let endpoint = format!("/databases/{}/tables/{}", database, table);
        self.make_request(reqwest::Method::PUT, &endpoint, Some(request))
            .await
    }

    /// Delete data from a table.
    ///
    /// # Arguments
    ///
    /// * `database` - Name of the database
    /// * `table` - Name of the table
    /// * `condition` - Condition for filtering data to delete (optional)
    ///
    /// # Returns
    ///
    /// Response from the server
    pub async fn delete(
        &self,
        database: &str,
        table: &str,
        condition: Option<&str>,
    ) -> Result<serde_json::Value, PhantomDBError> {
        let mut endpoint = format!("/databases/{}/tables/{}", database, table);
        if let Some(condition) = condition {
            endpoint.push_str(&format!("?condition={}", condition));
        }
        
        self.make_request(reqwest::Method::DELETE, &endpoint, None::<()>)
            .await
    }

    /// Execute a custom SQL query.
    ///
    /// # Arguments
    ///
    /// * `database` - Name of the database
    /// * `query` - SQL query to execute
    ///
    /// # Returns
    ///
    /// Query result
    pub async fn execute_query(
        &self,
        database: &str,
        query: &str,
    ) -> Result<serde_json::Value, PhantomDBError> {
        #[derive(Serialize)]
        struct ExecuteQueryRequest {
            query: String,
        }
        
        let request = ExecuteQueryRequest {
            query: query.to_string(),
        };
        
        let endpoint = format!("/databases/{}/query", database);
        self.make_request(reqwest::Method::POST, &endpoint, Some(request))
            .await
    }

    /// Begin a new transaction.
    ///
    /// # Returns
    ///
    /// Transaction ID
    pub async fn begin_transaction(&self) -> Result<String, PhantomDBError> {
        #[derive(Deserialize)]
        struct BeginTransactionResponse {
            transaction_id: String,
        }
        
        let result: BeginTransactionResponse = self
            .make_request(reqwest::Method::POST, "/transactions", None::<()>)
            .await?;
        Ok(result.transaction_id)
    }

    /// Commit a transaction.
    ///
    /// # Arguments
    ///
    /// * `transaction_id` - ID of the transaction to commit
    ///
    /// # Returns
    ///
    /// Response from the server
    pub async fn commit_transaction(&self, transaction_id: &str) -> Result<serde_json::Value, PhantomDBError> {
        let endpoint = format!("/transactions/{}/commit", transaction_id);
        self.make_request(reqwest::Method::POST, &endpoint, None::<()>)
            .await
    }

    /// Rollback a transaction.
    ///
    /// # Arguments
    ///
    /// * `transaction_id` - ID of the transaction to rollback
    ///
    /// # Returns
    ///
    /// Response from the server
    pub async fn rollback_transaction(&self, transaction_id: &str) -> Result<serde_json::Value, PhantomDBError> {
        let endpoint = format!("/transactions/{}/rollback", transaction_id);
        self.make_request(reqwest::Method::POST, &endpoint, None::<()>)
            .await
    }
}