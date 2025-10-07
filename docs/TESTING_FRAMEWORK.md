# PhantomDB Testing Framework

## Overview

The PhantomDB Testing Framework provides comprehensive utilities for testing PhantomDB applications. It offers base test case classes, decorators, context managers, and assertion utilities to make database testing easier and more reliable.

## Features

### 1. Test Case Base Classes
- `PhantomDBTestCase` - Synchronous test case with automatic database management
- `AsyncPhantomDBTestCase` - Asynchronous test case for async operations

### 2. Test Decorators
- `@phantomdb_test` - Automatic test database creation and cleanup
- `@async_phantomdb_test` - Async version of the test decorator

### 3. Context Managers
- `phantomdb_transaction` - Automatic transaction rollback for test isolation

### 4. Assertion Utilities
- `assert_table_exists` - Verify table existence
- `assert_row_count` - Check row counts
- `assert_row_exists` - Verify row existence with conditions

### 5. Test Runner
- `PhantomDBTestRunner` - Custom test runner with detailed reporting

## Installation

To use the testing framework, install the required dependencies:

```bash
pip install -r src/testing/requirements.txt
```

## Usage Examples

### Basic Test Case

```python
import unittest
from phantomdb.testing import PhantomDBTestCase

class TestUserOperations(PhantomDBTestCase):
    def setUp(self):
        super().setUp()
        # Create a users table for testing
        self.create_table("users", [
            {"name": "id", "type": "int"},
            {"name": "name", "type": "string"},
            {"name": "email", "type": "string"}
        ])
    
    def test_user_creation(self):
        # Insert a user
        self.insert_data("users", {
            "id": 1,
            "name": "John Doe",
            "email": "john@example.com"
        })
        
        # Assert the user was created
        self.assert_row_count("users", 1)
        self.assert_row_exists("users", "id = 1")
```

### Using Decorators

```python
from phantomdb.testing import phantomdb_test

@phantomdb_test()
def test_database_operations(test_database):
    # The test_database is automatically created and cleaned up
    client = PhantomDB("http://localhost:8080")
    
    # Create a table
    client.create_table(test_database, "products", [
        {"name": "id", "type": "int"},
        {"name": "name", "type": "string"},
        {"name": "price", "type": "float"}
    ])
    
    # Insert data
    client.insert(test_database, "products", {
        "id": 1,
        "name": "Test Product",
        "price": 9.99
    })
    
    # Verify data
    products = client.select(test_database, "products")
    assert len(products) == 1
```

### Using Context Managers

```python
from phantomdb.testing import phantomdb_transaction

def test_transaction_rollback():
    with phantomdb_transaction() as txn_id:
        # All operations in this block will be rolled back
        client = PhantomDB("http://localhost:8080")
        
        # Create a table
        client.create_table(txn_id, "temp_table", [
            {"name": "id", "type": "int"},
            {"name": "value", "type": "string"}
        ])
        
        # Insert data
        client.insert(txn_id, "temp_table", {"id": 1, "value": "test"})
        
        # The table and data will be automatically rolled back
```

### Async Tests

```python
from phantomdb.testing import AsyncPhantomDBTestCase

class TestAsyncOperations(AsyncPhantomDBTestCase):
    async def asyncSetUp(self):
        await super().asyncSetUp()
        # Create test tables
        await self.create_table("async_users", [
            {"name": "id", "type": "int"},
            {"name": "name", "type": "string"}
        ])
    
    async def test_async_user_creation(self):
        # Insert a user asynchronously
        await self.insert_data("async_users", {
            "id": 1,
            "name": "Async User"
        })
        
        # Assert the user was created
        await self.assert_row_count("async_users", 1)
```

## Integration with Testing Frameworks

### Pytest Integration

```python
import pytest
from phantomdb.testing import phantomdb_test

@phantomdb_test()
def test_with_pytest(test_database):
    # This test will work with pytest
    assert test_database.startswith("test_db_")
```

### Unittest Integration

```python
import unittest
from phantomdb.testing import PhantomDBTestCase

class TestWithUnittest(PhantomDBTestCase):
    def test_database_creation(self):
        # This test will work with unittest
        self.assertIsNotNone(self.test_database)
```

## API Reference

### PhantomDBTestCase

#### Methods
- `setUp()` - Initialize test database and client
- `tearDown()` - Clean up test database
- `create_table(table_name, columns)` - Create a table
- `insert_data(table_name, data)` - Insert data into a table
- `select_data(table_name, condition)` - Select data from a table
- `assert_table_exists(table_name)` - Assert table exists
- `assert_row_count(table_name, expected_count)` - Assert row count
- `assert_row_exists(table_name, condition)` - Assert row exists

### Decorators

#### @phantomdb_test
```python
@phantomdb_test(database_url="http://localhost:8080")
def test_function(test_database):
    # test_database is automatically created and cleaned up
    pass
```

#### @async_phantomdb_test
```python
@async_phantomdb_test(database_url="http://localhost:8080")
async def test_function(test_database):
    # test_database is automatically created and cleaned up
    pass
```

### Context Managers

#### phantomdb_transaction
```python
with phantomdb_transaction(database_url="http://localhost:8080") as txn_id:
    # All operations are automatically rolled back
    pass
```

## Best Practices

1. **Isolate Tests**: Each test should use its own database or transaction
2. **Clean Up Resources**: Use the provided decorators and context managers
3. **Use Assertions**: Leverage the built-in assertion utilities
4. **Handle Errors**: Test both success and failure cases
5. **Mock External Dependencies**: Use mocking for external services

## Requirements

- Python 3.7 or higher
- phantomdb-python client
- pytest (optional, for pytest integration)
- unittest2 (for enhanced unittest features)