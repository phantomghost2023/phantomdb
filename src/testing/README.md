# PhantomDB Testing Framework

A comprehensive testing framework for PhantomDB applications that provides utilities for testing database operations, transactions, and integration with popular testing frameworks.

## Features

1. **Test Case Base Classes**
   - Synchronous and asynchronous test case classes
   - Automatic test database creation and cleanup
   - Table and data management utilities

2. **Test Decorators**
   - `@phantomdb_test` for automatic test database management
   - `@async_phantomdb_test` for async tests

3. **Context Managers**
   - `phantomdb_transaction` for automatic transaction rollback

4. **Assertion Utilities**
   - Table existence checks
   - Row count assertions
   - Data validation helpers

5. **Test Runner**
   - Custom test runner with detailed reporting
   - Integration with unittest and pytest

## Installation

```bash
pip install -e .
```

## Usage

### Basic Test Case

```python
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

## Running Tests

```bash
# Run with unittest
python -m unittest discover -s tests

# Run with pytest
pytest tests/

# Run specific test file
python -m unittest tests.test_example
```

## Requirements

- Python 3.7 or higher
- phantomdb-python client
- pytest (optional, for pytest integration)
- unittest2 (for enhanced unittest features)