# PhantomDB Python Client

A Python client library for interacting with PhantomDB through its REST API.

## Installation

```bash
pip install phantomdb-client
```

Or install directly from source:

```bash
pip install -e .
```

## Usage

### Basic Usage

```python
from phantomdb import PhantomDB

# Connect to PhantomDB server
db = PhantomDB("http://localhost:8080")

# Check server health
health = db.health_check()
print(health)

# Create a database
db.create_database("myapp")

# List databases
databases = db.list_databases()
print(databases)

# Create a table
columns = [
    {"name": "id", "type": "int"},
    {"name": "name", "type": "string"},
    {"name": "email", "type": "string"}
]
db.create_table("myapp", "users", columns)

# Insert data
user_data = {
    "id": "1",
    "name": "John Doe",
    "email": "john@example.com"
}
db.insert("myapp", "users", user_data)

# Query data
users = db.select("myapp", "users")
print(users)

# Update data
db.update("myapp", "users", {"name": "Jane Doe"}, "id = 1")

# Delete data
db.delete("myapp", "users", "id = 1")

# Drop table
db.drop_table("myapp", "users")

# Drop database
db.drop_database("myapp")
```

### Transactions

```python
# Begin a transaction
txn_id = db.begin_transaction()

try:
    # Perform operations within the transaction
    db.insert("myapp", "users", {"id": "1", "name": "John Doe"})
    db.insert("myapp", "users", {"id": "2", "name": "Jane Smith"})
    
    # Commit the transaction
    db.commit_transaction(txn_id)
except Exception as e:
    # Rollback on error
    db.rollback_transaction(txn_id)
    raise
```

### Custom Queries

```python
# Execute custom SQL queries
result = db.execute_query("myapp", "SELECT * FROM users WHERE name LIKE 'J%'")
print(result)
```

## API Reference

### Database Operations

- `health_check()` - Check server health
- `list_databases()` - List all databases
- `create_database(name)` - Create a new database
- `drop_database(name)` - Drop a database

### Table Operations

- `list_tables(database)` - List tables in a database
- `create_table(database, name, columns)` - Create a new table
- `drop_table(database, name)` - Drop a table

### Data Operations

- `insert(database, table, data)` - Insert data into a table
- `select(database, table, condition=None)` - Select data from a table
- `update(database, table, data, condition=None)` - Update data in a table
- `delete(database, table, condition=None)` - Delete data from a table

### Query Operations

- `execute_query(database, query)` - Execute a custom SQL query

### Transaction Operations

- `begin_transaction()` - Begin a new transaction
- `commit_transaction(transaction_id)` - Commit a transaction
- `rollback_transaction(transaction_id)` - Rollback a transaction

## Requirements

- Python 3.7 or higher
- requests library

## License

MIT License