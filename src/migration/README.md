# PhantomDB Migration and Schema Management Utilities

This package provides tools for managing database schema migrations and versioning in PhantomDB.

## Features

- Database schema migration management
- Version control for database changes
- Schema export and import utilities
- Command-line interface for common operations

## Installation

```bash
pip install -r requirements.txt
```

## Usage

### Command Line Interface

```bash
# Create a new migration
python cli.py create "add_users_table" --description "Add users table to database"

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

### Python API

```python
from migration_manager import MigrationManager
from schema_manager import SchemaManager

# Initialize managers
migration_manager = MigrationManager("migrations")
schema_manager = SchemaManager()

# Create a new migration
migration_manager.create_migration("add_users_table", "Add users table")

# Apply all pending migrations
migration_manager.migrate(db)

# Export current schema
schema_manager.export_schema(db, "schema.json")
```

## Migration Files

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

## Schema Files

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