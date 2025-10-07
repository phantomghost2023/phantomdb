//! Connection pooling for PhantomDB Rust client
//!
//! This module provides connection pooling functionality for PhantomDB clients.

use std::collections::HashMap;
use std::sync::{Arc, Mutex};
use std::time::{Duration, Instant};
use tokio::sync::{Mutex as TokioMutex, Semaphore};
use tokio::time::timeout;

use crate::{PhantomDB, PhantomDBError};

/// Configuration for a connection pool
#[derive(Debug, Clone)]
pub struct PoolConfig {
    /// Base URL for PhantomDB connections
    pub base_url: String,
    /// Minimum number of connections in the pool
    pub min_connections: usize,
    /// Maximum number of connections in the pool
    pub max_connections: usize,
    /// Timeout for acquiring a connection
    pub acquire_timeout: Duration,
    /// Timeout for idle connections
    pub idle_timeout: Duration,
}

impl Default for PoolConfig {
    fn default() -> Self {
        Self {
            base_url: "http://localhost:8080".to_string(),
            min_connections: 2,
            max_connections: 10,
            acquire_timeout: Duration::from_secs(30),
            idle_timeout: Duration::from_secs(60),
        }
    }
}

/// A connection pool for PhantomDB clients
pub struct PhantomDBPool {
    config: PoolConfig,
    pool: Arc<Mutex<Vec<(PhantomDB, Instant)>>>,
    used: Arc<Mutex<HashMap<usize, PhantomDB>>>,
    semaphore: Arc<Semaphore>,
    closed: Arc<Mutex<bool>>,
}

impl PhantomDBPool {
    /// Create a new connection pool with the given configuration
    pub fn new(config: PoolConfig) -> Self {
        let pool = Arc::new(Mutex::new(Vec::new()));
        let used = Arc::new(Mutex::new(HashMap::new()));
        let semaphore = Arc::new(Semaphore::new(config.max_connections));
        let closed = Arc::new(Mutex::new(false));
        
        let pool_instance = Self {
            config,
            pool: pool.clone(),
            used: used.clone(),
            semaphore: semaphore.clone(),
            closed: closed.clone(),
        };
        
        // Initialize the pool
        pool_instance.initialize_pool();
        
        pool_instance
    }
    
    /// Initialize the connection pool with minimum connections
    fn initialize_pool(&self) {
        let mut pool = self.pool.lock().unwrap();
        for _ in 0..self.config.min_connections {
            if let Ok(connection) = self.create_connection() {
                pool.push((connection, Instant::now()));
            }
        }
    }
    
    /// Create a new connection
    fn create_connection(&self) -> Result<PhantomDB, PhantomDBError> {
        Ok(PhantomDB::new(&self.config.base_url))
    }
    
    /// Acquire a connection from the pool
    pub async fn acquire(&self) -> Result<PooledConnection, PhantomDBError> {
        // Check if pool is closed
        {
            let closed = self.closed.lock().unwrap();
            if *closed {
                return Err(PhantomDBError::ConnectionError("Connection pool is closed".to_string()));
            }
        }
        
        // Acquire a permit from the semaphore
        let _permit = timeout(self.config.acquire_timeout, self.semaphore.acquire())
            .await
            .map_err(|_| PhantomDBError::ConnectionError("Timeout acquiring connection permit".to_string()))?
            .map_err(|_| PhantomDBError::ConnectionError("Failed to acquire connection permit".to_string()))?;
        
        // Try to get an available connection
        let mut pool = self.pool.lock().unwrap();
        while let Some((connection, last_used)) = pool.pop() {
            // Check if connection is still valid
            if last_used.elapsed() < self.config.idle_timeout {
                // Add to used connections
                let connection_id = connection as *const _ as usize;
                let mut used = self.used.lock().unwrap();
                used.insert(connection_id, connection);
                
                return Ok(PooledConnection {
                    connection: Some(connection),
                    connection_id,
                    pool: self.clone(),
                    _permit,
                });
            }
            // Connection is too old, discard it and continue
        }
        
        // No available connections, create a new one
        drop(pool); // Release the lock before creating a new connection
        let connection = self.create_connection()?;
        let connection_id = connection as *const _ as usize;
        
        // Add to used connections
        let mut used = self.used.lock().unwrap();
        used.insert(connection_id, connection);
        
        Ok(PooledConnection {
            connection: Some(connection),
            connection_id,
            pool: self.clone(),
            _permit,
        })
    }
    
    /// Release a connection back to the pool
    fn release(&self, connection_id: usize, connection: PhantomDB) {
        let closed = self.closed.lock().unwrap();
        if *closed {
            return;
        }
        drop(closed);
        
        // Remove from used connections
        let mut used = self.used.lock().unwrap();
        if used.remove(&connection_id).is_some() {
            // Add back to pool
            let mut pool = self.pool.lock().unwrap();
            pool.push((connection, Instant::now()));
        }
    }
    
    /// Close the connection pool
    pub fn close(&self) {
        let mut closed = self.closed.lock().unwrap();
        *closed = true;
        
        // Clear the pool
        let mut pool = self.pool.lock().unwrap();
        pool.clear();
        
        let mut used = self.used.lock().unwrap();
        used.clear();
    }
    
    /// Get the current pool status
    pub fn status(&self) -> PoolStatus {
        let pool = self.pool.lock().unwrap();
        let used = self.used.lock().unwrap();
        
        PoolStatus {
            available: pool.len(),
            used: used.len(),
            min: self.config.min_connections,
            max: self.config.max_connections,
        }
    }
}

impl Clone for PhantomDBPool {
    fn clone(&self) -> Self {
        Self {
            config: self.config.clone(),
            pool: self.pool.clone(),
            used: self.used.clone(),
            semaphore: self.semaphore.clone(),
            closed: self.closed.clone(),
        }
    }
}

/// A pooled connection that automatically returns to the pool when dropped
pub struct PooledConnection {
    connection: Option<PhantomDB>,
    connection_id: usize,
    pool: PhantomDBPool,
    _permit: tokio::sync::OwnedSemaphorePermit,
}

impl PooledConnection {
    /// Get a reference to the underlying PhantomDB client
    pub fn inner(&self) -> &PhantomDB {
        self.connection.as_ref().unwrap()
    }
    
    /// Get a mutable reference to the underlying PhantomDB client
    pub fn inner_mut(&mut self) -> &mut PhantomDB {
        self.connection.as_mut().unwrap()
    }
}

impl Drop for PooledConnection {
    fn drop(&mut self) {
        if let Some(connection) = self.connection.take() {
            self.pool.release(self.connection_id, connection);
        }
    }
}

/// Status information for a connection pool
#[derive(Debug)]
pub struct PoolStatus {
    /// Number of available connections
    pub available: usize,
    /// Number of used connections
    pub used: usize,
    /// Minimum number of connections
    pub min: usize,
    /// Maximum number of connections
    pub max: usize,
}

/// Create a new connection pool with default configuration
pub fn create_pool() -> PhantomDBPool {
    PhantomDBPool::new(PoolConfig::default())
}

/// Create a new connection pool with custom configuration
pub fn create_pool_with_config(config: PoolConfig) -> PhantomDBPool {
    PhantomDBPool::new(config)
}