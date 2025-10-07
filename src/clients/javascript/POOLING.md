# PhantomDB JavaScript Connection Pooling

This document provides documentation for the connection pooling functionality in the PhantomDB JavaScript client.

## Overview

The PhantomDB JavaScript connection pool provides a way to manage and reuse connections to the PhantomDB server, improving performance by reducing the overhead of creating new connections for each operation.

## Features

- Connection reuse to reduce overhead
- Configurable pool size
- Automatic connection validation
- Timeout management
- Thread-safe operations

## Installation

The connection pooling functionality is included in the main PhantomDB JavaScript client package.

## Usage

### Creating a Connection Pool

```javascript
const { createPool } = require('phantomdb/pool');

// Create a connection pool with default settings
const pool = createPool({
  baseUrl: 'http://localhost:8080'
});

// Create a connection pool with custom settings
const customPool = createPool({
  baseUrl: 'http://localhost:8080',
  minConnections: 5,
  maxConnections: 20,
  acquireTimeout: 60000, // 60 seconds
  idleTimeout: 300000    // 5 minutes
});
```

### Acquiring and Releasing Connections

```javascript
async function performDatabaseOperations() {
  // Acquire a connection from the pool
  const client = await pool.acquire();
  
  try {
    // Use the client for database operations
    await client.createDatabase('myapp');
    const databases = await client.listDatabases();
    console.log('Databases:', databases);
    
    // Perform other operations...
  } finally {
    // Always release the connection back to the pool
    pool.release(client);
  }
}
```

### Using Connection Pool with Async/Await

```javascript
async function exampleWithPool() {
  const pool = createPool({ baseUrl: 'http://localhost:8080' });
  
  try {
    // Acquire connection
    const client = await pool.acquire();
    
    // Perform operations
    const result = await client.createDatabase('testdb');
    console.log('Database created:', result);
    
    // Release connection
    pool.release(client);
  } catch (error) {
    console.error('Error:', error.message);
  } finally {
    // Close the pool when done
    await pool.close();
  }
}
```

### Pool Status Monitoring

```javascript
const pool = createPool({ baseUrl: 'http://localhost:8080' });

// Get current pool status
const status = pool.getStatus();
console.log('Pool status:', status);
// Output: { available: 2, used: 0, pending: 0, min: 2, max: 10 }
```

## Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `baseUrl` | `'http://localhost:8080'` | The base URL of the PhantomDB REST API server |
| `minConnections` | `2` | Minimum number of connections in the pool |
| `maxConnections` | `10` | Maximum number of connections in the pool |
| `acquireTimeout` | `30000` | Timeout for acquiring a connection (milliseconds) |
| `idleTimeout` | `60000` | Timeout for idle connections (milliseconds) |

## Error Handling

The connection pool provides proper error handling for various scenarios:

```javascript
async function handlePoolErrors() {
  const pool = createPool({ baseUrl: 'http://localhost:8080' });
  
  try {
    // This will throw if no connection is available within the timeout
    const client = await pool.acquire();
    // ... use client
    pool.release(client);
  } catch (error) {
    if (error.message.includes('Timeout')) {
      console.error('Unable to acquire connection within timeout');
    } else if (error.message.includes('closed')) {
      console.error('Connection pool is closed');
    } else {
      console.error('Connection error:', error.message);
    }
  }
}
```

## Best Practices

1. **Always release connections**: Make sure to release connections back to the pool after use
2. **Use try/finally blocks**: Ensure connections are released even if an error occurs
3. **Close the pool**: Close the pool when your application shuts down
4. **Monitor pool status**: Use `getStatus()` to monitor pool performance
5. **Configure appropriately**: Set pool sizes based on your application's needs

## API Reference

### `createPool(options)`

Creates a new connection pool.

**Parameters:**
- `options` (Object): Configuration options

**Returns:**
- `PhantomDBConnectionPool`: A new connection pool instance

### `PhantomDBConnectionPool`

#### `acquire()`

Acquires a connection from the pool.

**Returns:**
- `Promise<Object>`: A PhantomDB client instance

#### `release(connection)`

Releases a connection back to the pool.

**Parameters:**
- `connection` (Object): The PhantomDB client instance to release

#### `close()`

Closes the connection pool and all connections.

#### `getStatus()`

Gets the current pool status.

**Returns:**
- `Object`: Pool status information

## Examples

### Web Server Integration

```javascript
const express = require('express');
const { createPool } = require('phantomdb/pool');

const app = express();
const pool = createPool({ baseUrl: 'http://localhost:8080' });

app.get('/databases', async (req, res) => {
  let client;
  try {
    client = await pool.acquire();
    const databases = await client.listDatabases();
    res.json(databases);
  } catch (error) {
    res.status(500).json({ error: error.message });
  } finally {
    if (client) {
      pool.release(client);
    }
  }
});

app.listen(3000, () => {
  console.log('Server running on port 3000');
});

// Close pool on shutdown
process.on('SIGINT', async () => {
  await pool.close();
  process.exit(0);
});
```

## Testing

The connection pool includes comprehensive tests:

```javascript
// test/pool.test.js
const { createPool } = require('../pool');

describe('PhantomDB Connection Pool', () => {
  test('should create pool with default settings', () => {
    const pool = createPool({ baseUrl: 'http://localhost:8080' });
    const status = pool.getStatus();
    expect(status.min).toBe(2);
    expect(status.max).toBe(10);
  });
  
  test('should acquire and release connections', async () => {
    const pool = createPool({ baseUrl: 'http://localhost:8080' });
    const client = await pool.acquire();
    expect(client).toBeDefined();
    pool.release(client);
  });
});
```

## License

MIT License