/**
 * PhantomDB Connection Pool for JavaScript
 * 
 * This module provides connection pooling functionality for PhantomDB clients.
 */

class PhantomDBConnectionPool {
  /**
   * Create a new connection pool.
   * 
   * @param {Object} options - Pool configuration options
   * @param {string} options.baseUrl - The base URL of the PhantomDB REST API server
   * @param {number} [options.minConnections=2] - Minimum number of connections in the pool
   * @param {number} [options.maxConnections=10] - Maximum number of connections in the pool
   * @param {number} [options.acquireTimeout=30000] - Timeout for acquiring a connection (ms)
   * @param {number} [options.idleTimeout=60000] - Timeout for idle connections (ms)
   */
  constructor(options = {}) {
    this.baseUrl = options.baseUrl || 'http://localhost:8080';
    this.minConnections = options.minConnections || 2;
    this.maxConnections = options.maxConnections || 10;
    this.acquireTimeout = options.acquireTimeout || 30000;
    this.idleTimeout = options.idleTimeout || 60000;
    
    // Pool state
    this.pool = [];
    this.used = new Set();
    this.pending = [];
    this.closed = false;
    
    // Initialize the pool
    this._initializePool();
  }
  
  /**
   * Initialize the connection pool with minimum connections.
   * @private
   */
  _initializePool() {
    for (let i = 0; i < this.minConnections; i++) {
      this._createConnection();
    }
  }
  
  /**
   * Create a new connection and add it to the pool.
   * @private
   */
  _createConnection() {
    if (this.pool.length + this.used.size >= this.maxConnections) {
      return null;
    }
    
    const { PhantomDB } = require('./phantomdb');
    const connection = new PhantomDB(this.baseUrl);
    this.pool.push({
      client: connection,
      lastUsed: Date.now(),
      idle: true
    });
    
    return connection;
  }
  
  /**
   * Acquire a connection from the pool.
   * 
   * @returns {Promise<Object>} A PhantomDB client instance
   * @throws {Error} If unable to acquire a connection within the timeout
   */
  async acquire() {
    if (this.closed) {
      throw new Error('Connection pool is closed');
    }
    
    // Try to get an available connection
    while (this.pool.length > 0) {
      const connectionInfo = this.pool.pop();
      if (Date.now() - connectionInfo.lastUsed < this.idleTimeout) {
        connectionInfo.idle = false;
        this.used.add(connectionInfo);
        return connectionInfo.client;
      }
      // Connection is too old, discard it
    }
    
    // If we're at max capacity, wait for a connection to be released
    if (this.used.size >= this.maxConnections) {
      return new Promise((resolve, reject) => {
        const timeout = setTimeout(() => {
          reject(new Error('Timeout acquiring connection from pool'));
        }, this.acquireTimeout);
        
        this.pending.push({ resolve, reject, timeout });
      });
    }
    
    // Create a new connection
    const connection = this._createConnection();
    if (connection) {
      const connectionInfo = this.pool.find(info => info.client === connection);
      if (connectionInfo) {
        connectionInfo.idle = false;
        this.used.add(connectionInfo);
      }
      return connection;
    }
    
    throw new Error('Unable to acquire connection');
  }
  
  /**
   * Release a connection back to the pool.
   * 
   * @param {Object} connection - The PhantomDB client instance to release
   */
  release(connection) {
    if (this.closed) {
      return;
    }
    
    // Find the connection info
    const connectionInfo = Array.from(this.used).find(info => info.client === connection);
    if (connectionInfo) {
      // Remove from used set
      this.used.delete(connectionInfo);
      
      // Update last used time
      connectionInfo.lastUsed = Date.now();
      connectionInfo.idle = true;
      
      // Add back to pool
      this.pool.push(connectionInfo);
      
      // Check if there are pending requests
      if (this.pending.length > 0) {
        const pending = this.pending.shift();
        clearTimeout(pending.timeout);
        this.acquire().then(pending.resolve).catch(pending.reject);
      }
    }
  }
  
  /**
   * Close the connection pool and all connections.
   */
  async close() {
    this.closed = true;
    
    // Clear pending requests
    while (this.pending.length > 0) {
      const pending = this.pending.shift();
      clearTimeout(pending.timeout);
      pending.reject(new Error('Connection pool closed'));
    }
    
    // Clear the pool
    this.pool = [];
    this.used.clear();
  }
  
  /**
   * Get the current pool status.
   * 
   * @returns {Object} Pool status information
   */
  getStatus() {
    return {
      available: this.pool.length,
      used: this.used.size,
      pending: this.pending.length,
      min: this.minConnections,
      max: this.maxConnections
    };
  }
}

/**
 * Create a new PhantomDB connection pool.
 * 
 * @param {Object} options - Pool configuration options
 * @returns {PhantomDBConnectionPool} A new connection pool instance
 */
function createPool(options) {
  return new PhantomDBConnectionPool(options);
}

module.exports = {
  PhantomDBConnectionPool,
  createPool
};