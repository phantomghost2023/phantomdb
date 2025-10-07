# PhantomDB Python Connection Pooling

This document provides documentation for the connection pooling functionality in the PhantomDB Python client.

## Overview

The PhantomDB Python connection pool provides a way to manage and reuse connections to the PhantomDB server, improving performance by reducing the overhead of creating new connections for each operation.

## Features

- Connection reuse to reduce overhead
- Configurable pool size
- Automatic connection validation
- Timeout management
- Thread-safe operations

## Installation

The connection pooling functionality is included in the main PhantomDB Python client package.

## Usage

### Creating a Connection Pool

```python
from phantomdb.pool import create_pool

# Create a connection pool with default settings
pool = create_pool(base_url='http://localhost:8080')

# Create a connection pool with custom settings
custom_pool = create_pool(
    base_url='http://localhost:8080',
    min_connections=5,
    max_connections=20,
    acquire_timeout=60,  # 60 seconds
    idle_timeout=300     # 5 minutes
)
```

### Acquiring and Releasing Connections

```python
def perform_database_operations():
    # Acquire a connection from the pool
    client = pool.acquire()
    
    try:
        # Use the client for database operations
        client.create_database('myapp')
        databases = client.list_databases()
        print('Databases:', databases)
        
        # Perform other operations...
    finally:
        # Always release the connection back to the pool
        pool.release(client)
```

### Using Connection Pool with Context Managers

```python
from contextlib import contextmanager

@contextmanager
def get_db_connection(pool):
    client = pool.acquire()
    try:
        yield client
    finally:
        pool.release(client)

def example_with_context_manager():
    pool = create_pool(base_url='http://localhost:8080')
    
    with get_db_connection(pool) as client:
        result = client.create_database('testdb')
        print('Database created:', result)
    
    # Pool is automatically closed when done
    pool.close()
```

### Pool Status Monitoring

```python
pool = create_pool(base_url='http://localhost:8080')

# Get current pool status
status = pool.get_status()
print('Pool status:', status)
# Output: {'available': 2, 'used': 0, 'min': 2, 'max': 10}
```

## Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `base_url` | `'http://localhost:8080'` | The base URL of the PhantomDB REST API server |
| `min_connections` | `2` | Minimum number of connections in the pool |
| `max_connections` | `10` | Maximum number of connections in the pool |
| `acquire_timeout` | `30` | Timeout for acquiring a connection (seconds) |
| `idle_timeout` | `60` | Timeout for idle connections (seconds) |

## Error Handling

The connection pool provides proper error handling for various scenarios:

```python
def handle_pool_errors():
    pool = create_pool(base_url='http://localhost:8080')
    
    try:
        # This will raise PhantomDBError if no connection is available within the timeout
        client = pool.acquire()
        # ... use client
        pool.release(client)
    except PhantomDBError as error:
        if 'Timeout' in str(error):
            print('Unable to acquire connection within timeout')
        elif 'closed' in str(error):
            print('Connection pool is closed')
        else:
            print('Connection error:', error)
```

## Best Practices

1. **Always release connections**: Make sure to release connections back to the pool after use
2. **Use context managers**: Use context managers to ensure connections are released even if an error occurs
3. **Close the pool**: Close the pool when your application shuts down
4. **Monitor pool status**: Use `get_status()` to monitor pool performance
5. **Configure appropriately**: Set pool sizes based on your application's needs

## API Reference

### `create_pool(base_url, min_connections, max_connections, acquire_timeout, idle_timeout)`

Creates a new connection pool.

**Parameters:**
- `base_url` (str): The base URL of the PhantomDB REST API server
- `min_connections` (int): Minimum number of connections in the pool
- `max_connections` (int): Maximum number of connections in the pool
- `acquire_timeout` (int): Timeout for acquiring a connection (seconds)
- `idle_timeout` (int): Timeout for idle connections (seconds)

**Returns:**
- `PhantomDBConnectionPool`: A new connection pool instance

### `PhantomDBConnectionPool`

#### `acquire()`

Acquires a connection from the pool.

**Returns:**
- `PhantomDB`: A PhantomDB client instance

**Raises:**
- `PhantomDBError`: If unable to acquire a connection within the timeout

#### `release(connection)`

Releases a connection back to the pool.

**Parameters:**
- `connection` (PhantomDB): The PhantomDB client instance to release

#### `close()`

Closes the connection pool and all connections.

#### `get_status()`

Gets the current pool status.

**Returns:**
- `dict`: Pool status information

## Examples

### Web Server Integration

```python
from flask import Flask
from phantomdb.pool import create_pool

app = Flask(__name__)
pool = create_pool(base_url='http://localhost:8080')

@app.route('/databases')
def list_databases():
    client = pool.acquire()
    try:
        databases = client.list_databases()
        return {'databases': databases}
    except Exception as error:
        return {'error': str(error)}, 500
    finally:
        pool.release(client)

if __name__ == '__main__':
    app.run()
    
    # Close pool on shutdown
    import atexit
    atexit.register(pool.close)
```

### Multi-threaded Application

```python
import threading
import time
from phantomdb.pool import create_pool

def worker(pool, worker_id):
    for i in range(5):
        client = pool.acquire()
        try:
            databases = client.list_databases()
            print(f'Worker {worker_id}, iteration {i}: Found {len(databases)} databases')
            time.sleep(0.1)  # Simulate work
        finally:
            pool.release(client)

def run_multithreaded_example():
    pool = create_pool(base_url='http://localhost:8080')
    
    # Create multiple worker threads
    threads = []
    for i in range(3):
        thread = threading.Thread(target=worker, args=(pool, i))
        threads.append(thread)
        thread.start()
    
    # Wait for all threads to complete
    for thread in threads:
        thread.join()
    
    # Check final pool status
    status = pool.get_status()
    print('Final pool status:', status)
    
    pool.close()

if __name__ == '__main__':
    run_multithreaded_example()
```

## Testing

The connection pool includes comprehensive tests:

```python
# test/test_pool.py
import unittest
from phantomdb.pool import create_pool

class TestPhantomDBPool(unittest.TestCase):
    def test_create_pool_with_default_settings(self):
        pool = create_pool(base_url='http://localhost:8080')
        status = pool.get_status()
        self.assertEqual(status['min'], 2)
        self.assertEqual(status['max'], 10)
    
    def test_acquire_and_release_connections(self):
        pool = create_pool(base_url='http://localhost:8080')
        client = pool.acquire()
        self.assertIsNotNone(client)
        pool.release(client)
    
    def test_pool_status(self):
        pool = create_pool(base_url='http://localhost:8080')
        initial_status = pool.get_status()
        self.assertEqual(initial_status['available'], 2)
        self.assertEqual(initial_status['used'], 0)

if __name__ == '__main__':
    unittest.main()
```

## License

MIT License