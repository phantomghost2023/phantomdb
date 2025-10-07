# PhantomDB Administration Tools

This document describes the administration tools implemented for PhantomDB.

## 1. Web-based Management Console

A web-based interface for administering PhantomDB instances built with Flask.

### Features

- Dashboard with server health monitoring
- Database management (create, list, drop)
- Table management (create, list, drop)
- Data browsing and insertion
- SQL query interface
- Responsive UI with Bootstrap 5

### Components

- **Flask Application**: Main web server (`src/web/app.py`)
- **HTML Templates**: Jinja2 templates for UI (`src/web/templates/`)
- **Static Assets**: CSS, JavaScript, and images
- **API Endpoints**: RESTful API for AJAX operations

### Usage

```bash
# Navigate to web console directory
cd src/web

# Install dependencies
pip install -r requirements.txt

# Run the web server
python app.py
```

The web console will be available at `http://localhost:5000`.

## 2. Migration and Schema Management Utilities

Tools for managing database schema migrations and versioning.

### Features

- Database schema migration management
- Version control for database changes
- Schema export and import utilities
- Command-line interface for common operations

### Components

- **Migration Manager**: Handles migration creation and application (`src/migration/migration_manager.py`)
- **Schema Manager**: Manages schema operations and validation (`src/migration/schema_manager.py`)
- **Command-Line Interface**: CLI for migration operations (`src/migration/cli.py`)
- **Migration Files**: Python scripts defining schema changes (`src/migration/migrations/`)

### Usage

```bash
# Create a new migration
python cli.py create "add_users_table" --description "Add users table"

# Apply pending migrations
python cli.py migrate

# Revert the last migration
python cli.py rollback

# Check migration status
python cli.py status

# Export current database schema
python cli.py export --output schema.json

# Apply a schema file
python cli.py schema apply schema.json

# Validate a schema file
python cli.py schema validate schema.json
```

### Migration Files

Migration files are Python scripts that define how to upgrade or downgrade database schemas:

```python
"""
PhantomDB Migration: add_users_table
Created: 2023-01-01 12:00:00
Description: Add users table to database
"""

def upgrade(db):
    """Apply this migration."""
    db.create_table("users", {
        "id": "integer",
        "name": "string",
        "email": "string"
    })

def downgrade(db):
    """Revert this migration."""
    db.drop_table("users")
```

### Schema Files

Schema files are JSON documents that define the structure of databases and tables:

```json
{
  "version": "1.0",
  "exported_at": "2023-01-01T12:00:00",
  "databases": {
    "mydb": {
      "tables": {
        "users": {
          "columns": [
            {"name": "id", "type": "integer"},
            {"name": "name", "type": "string"},
            {"name": "email", "type": "string"}
          ],
          "indexes": [
            {"name": "idx_email", "columns": ["email"]}
          ],
          "constraints": [
            {"name": "pk_users", "type": "primary_key", "columns": ["id"]}
          ]
        }
      }
    }
  }
}
```

## 3. Python Client Library

A Python client library for interacting with PhantomDB.

### Features

- Easy-to-use API for database operations
- Connection management
- Error handling
- Compatible with migration tools

### Components

- **PhantomDB Class**: Main client interface (`src/clients/python/phantomdb/phantomdb.py`)
- **Error Classes**: Custom exception types (`src/clients/python/phantomdb/exceptions.py`)

## 4. Command-Line Interface (CLI)

A command-line tool for common PhantomDB operations.

### Features

- Database and table management
- Data operations
- Health checking
- Integration with migration tools

### Components

- **CLI Tool**: Main command-line interface (`src/tools/cli/phantomdb_cli.py`)

## Summary

These administration tools provide a comprehensive set of utilities for managing PhantomDB instances:

1. **Web-based Management Console**: GUI for interactive database administration
2. **Migration and Schema Management**: Version control for database schema changes
3. **Python Client Library**: Programmatic access to PhantomDB
4. **Command-Line Interface**: Terminal-based administration tool

All tools are designed to work together seamlessly and provide multiple ways to administer PhantomDB instances.