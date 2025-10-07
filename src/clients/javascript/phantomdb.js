/**
 * PhantomDB JavaScript Client
 * 
 * This module provides a JavaScript interface to interact with the PhantomDB REST API.
 */

class PhantomDBError extends Error {
  /**
   * Custom exception for PhantomDB client errors.
   * @param {string} message - Error message
   */
  constructor(message) {
    super(message);
    this.name = 'PhantomDBError';
  }
}

class PhantomDB {
  /**
   * A client for interacting with PhantomDB through its REST API.
   * 
   * This client provides methods for database operations such as creating databases,
   * tables, inserting data, querying data, etc.
   * 
   * @param {string} baseUrl - The base URL of the PhantomDB REST API server
   */
  constructor(baseUrl = 'http://localhost:8080') {
    this.baseUrl = baseUrl.replace(/\/$/, ''); // Remove trailing slash if present
  }

  /**
   * Make an HTTP request to the PhantomDB REST API.
   * 
   * @param {string} method - HTTP method (GET, POST, PUT, DELETE, etc.)
   * @param {string} endpoint - API endpoint
   * @param {Object} [data] - Data to send with the request
   * @returns {Promise<Object>} JSON response from the server
   * @throws {PhantomDBError} If the request fails
   */
  async _makeRequest(method, endpoint, data = null) {
    const url = `${this.baseUrl}${endpoint}`;
    
    try {
      const options = {
        method: method,
        headers: {
          'Content-Type': 'application/json'
        }
      };
      
      if (data !== null) {
        options.body = JSON.stringify(data);
      }
      
      const response = await fetch(url, options);
      
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      
      const content = await response.text();
      return content ? JSON.parse(content) : {};
    } catch (error) {
      throw new PhantomDBError(`Request failed: ${error.message}`);
    }
  }

  /**
   * Check the health of the PhantomDB server.
   * 
   * @returns {Promise<Object>} Health status information
   */
  async healthCheck() {
    return await this._makeRequest('GET', '/health');
  }

  /**
   * List all databases in PhantomDB.
   * 
   * @returns {Promise<Array<string>>} List of database names
   */
  async listDatabases() {
    const response = await this._makeRequest('GET', '/databases');
    return response.databases || [];
  }

  /**
   * Create a new database.
   * 
   * @param {string} name - Name of the database to create
   * @returns {Promise<Object>} Response from the server
   */
  async createDatabase(name) {
    const data = { name: name };
    return await this._makeRequest('POST', '/databases', data);
  }

  /**
   * Drop a database.
   * 
   * @param {string} name - Name of the database to drop
   * @returns {Promise<Object>} Response from the server
   */
  async dropDatabase(name) {
    return await this._makeRequest('DELETE', `/databases/${name}`);
  }

  /**
   * List all tables in a database.
   * 
   * @param {string} database - Name of the database
   * @returns {Promise<Array<string>>} List of table names
   */
  async listTables(database) {
    const response = await this._makeRequest('GET', `/databases/${database}/tables`);
    return response.tables || [];
  }

  /**
   * Create a new table in a database.
   * 
   * @param {string} database - Name of the database
   * @param {string} name - Name of the table to create
   * @param {Array<Object>} columns - List of column definitions
   * @returns {Promise<Object>} Response from the server
   */
  async createTable(database, name, columns) {
    const data = {
      name: name,
      columns: columns
    };
    return await this._makeRequest('POST', `/databases/${database}/tables`, data);
  }

  /**
   * Drop a table from a database.
   * 
   * @param {string} database - Name of the database
   * @param {string} name - Name of the table to drop
   * @returns {Promise<Object>} Response from the server
   */
  async dropTable(database, name) {
    return await this._makeRequest('DELETE', `/databases/${database}/tables/${name}`);
  }

  /**
   * Insert data into a table.
   * 
   * @param {string} database - Name of the database
   * @param {string} table - Name of the table
   * @param {Object} data - Data to insert
   * @returns {Promise<Object>} Response from the server
   */
  async insert(database, table, data) {
    return await this._makeRequest('POST', `/databases/${database}/tables/${table}`, data);
  }

  /**
   * Select data from a table.
   * 
   * @param {string} database - Name of the database
   * @param {string} table - Name of the table
   * @param {string} [condition] - Condition for filtering data
   * @returns {Promise<Array<Object>>} List of records matching the query
   */
  async select(database, table, condition = null) {
    let endpoint = `/databases/${database}/tables/${table}`;
    if (condition) {
      endpoint += `?condition=${encodeURIComponent(condition)}`;
    }
    
    const response = await this._makeRequest('GET', endpoint);
    return response.records || [];
  }

  /**
   * Update data in a table.
   * 
   * @param {string} database - Name of the database
   * @param {string} table - Name of the table
   * @param {Object} data - Data to update
   * @param {string} [condition] - Condition for filtering data to update
   * @returns {Promise<Object>} Response from the server
   */
  async update(database, table, data, condition = null) {
    const payload = { data: data };
    if (condition) {
      payload.condition = condition;
    }
    
    return await this._makeRequest('PUT', `/databases/${database}/tables/${table}`, payload);
  }

  /**
   * Delete data from a table.
   * 
   * @param {string} database - Name of the database
   * @param {string} table - Name of the table
   * @param {string} [condition] - Condition for filtering data to delete
   * @returns {Promise<Object>} Response from the server
   */
  async delete(database, table, condition = null) {
    let endpoint = `/databases/${database}/tables/${table}`;
    if (condition) {
      endpoint += `?condition=${encodeURIComponent(condition)}`;
    }
    
    return await this._makeRequest('DELETE', endpoint);
  }

  /**
   * Execute a custom SQL query.
   * 
   * @param {string} database - Name of the database
   * @param {string} query - SQL query to execute
   * @returns {Promise<Object>} Query result
   */
  async executeQuery(database, query) {
    const data = { query: query };
    return await this._makeRequest('POST', `/databases/${database}/query`, data);
  }

  /**
   * Begin a new transaction.
   * 
   * @returns {Promise<string>} Transaction ID
   */
  async beginTransaction() {
    const response = await this._makeRequest('POST', '/transactions');
    return response.transaction_id || '';
  }

  /**
   * Commit a transaction.
   * 
   * @param {string} transactionId - ID of the transaction to commit
   * @returns {Promise<Object>} Response from the server
   */
  async commitTransaction(transactionId) {
    return await this._makeRequest('POST', `/transactions/${transactionId}/commit`);
  }

  /**
   * Rollback a transaction.
   * 
   * @param {string} transactionId - ID of the transaction to rollback
   * @returns {Promise<Object>} Response from the server
   */
  async rollbackTransaction(transactionId) {
    return await this._makeRequest('POST', `/transactions/${transactionId}/rollback`);
  }
}

/**
 * Create a new PhantomDB client connection.
 * 
 * @param {string} baseUrl - The base URL of the PhantomDB REST API server
 * @returns {PhantomDB} A new PhantomDB client instance
 */
function connect(baseUrl = 'http://localhost:8080') {
  return new PhantomDB(baseUrl);
}

// Export for Node.js and browser environments
if (typeof module !== 'undefined' && module.exports) {
  // Node.js
  module.exports = { PhantomDB, PhantomDBError, connect };
} else if (typeof window !== 'undefined') {
  // Browser
  window.PhantomDB = { PhantomDB, PhantomDBError, connect };
}