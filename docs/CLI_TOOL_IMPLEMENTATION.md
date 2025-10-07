# CLI Tool Implementation

This document describes the implementation of the command line interface tool for PhantomDB as part of Phase 4: Developer Experience.

## Overview

The CLI tool provides a command line interface for administering PhantomDB instances. It allows system administrators and developers to perform database operations without needing to write custom code or use HTTP clients directly.

## Architecture

### Components

1. **PhantomDBCLI Class**: Main class that encapsulates all CLI functionality
2. **Command Line Parser**: Uses argparse to parse command line arguments
3. **HTTP Client**: Uses requests library to communicate with PhantomDB REST API
4. **Output Formatter**: Formats output for display to the user

### Key Features

- Object-oriented interface for CLI operations
- Comprehensive command line argument parsing
- Error handling with informative messages
- Support for all PhantomDB REST API endpoints
- Transaction management
- JSON output formatting

## Implementation Details

### PhantomDBCLI Class

The main CLI class provides methods for all PhantomDB operations:

```python
class PhantomDBCLI:
    def __init__(self, base_url: str = "http://localhost:8080"):
        # Initialize CLI with base URL
        pass
    
    def health_check(self) -> None:
        # Check server health
        pass
    
    def list_databases(self) -> None:
        # List all databases
        pass
    
    def create_database(self, name: str) -> None:
        # Create a new database
        pass
    
    # ... other methods
```

### Command Line Interface

The CLI uses argparse for command line parsing:

```python
def main():
    parser = argparse.ArgumentParser(description="PhantomDB Command Line Interface")
    subparsers = parser.add_subparsers(dest="command", help="Available commands")
    
    # Add parsers for each command
    # ...
```

## Commands

### Health Check

Check the health of the PhantomDB server:

```bash
phantomdb-cli health
```

### Database Operations

List all databases:

```bash
phantomdb-cli list-databases
```

Create a new database:

```bash
phantomdb-cli create-database <database_name>
```

Drop a database:

```bash
phantomdb-cli drop-database <database_name>
```

### Table Operations

List tables in a database:

```bash
phantomdb-cli list-tables <database_name>
```

Create a new table:

```bash
phantomdb-cli create-table <database_name> <table_name> <column1:type> <column2:type> ...
```

Drop a table:

```bash
phantomdb-cli drop-table <database_name> <table_name>
```

### Query Operations

Execute a custom SQL query:

```bash
phantomdb-cli execute-query <database_name> "<sql_query>"
```

### Transaction Operations

Begin a new transaction:

```bash
phantomdb-cli begin-transaction
```

Commit a transaction:

```bash
phantomdb-cli commit-transaction <transaction_id>
```

Rollback a transaction:

```bash
phantomdb-cli rollback-transaction <transaction_id>
```

## Usage Examples

### Basic Operations

```bash
# Check server health
phantomdb-cli health

# List databases
phantomdb-cli list-databases

# Create a database
phantomdb-cli create-database myapp
```

### Data Operations

```bash
# Create a table
phantomdb-cli create-table myapp users id:int name:string email:string

# List tables
phantomdb-cli list-tables myapp

# Execute a query
phantomdb-cli execute-query myapp "SELECT * FROM users"
```

### Transactions

```bash
# Begin a transaction
phantomdb-cli begin-transaction

# Commit a transaction
phantomdb-cli commit-transaction txn_12345

# Rollback a transaction
phantomdb-cli rollback-transaction txn_12345
```

## Testing

The CLI tool includes a comprehensive test suite using Python's unittest framework:

```python
class TestPhantomDBCLI(unittest.TestCase):
    def setUp(self):
        # Set up test fixtures
        pass
    
    @patch('phantomdb_cli.requests.Session.request')
    def test_health_check_success(self, mock_request):
        # Test health check functionality
        pass
```

## Installation

### From Source

```bash
cd src/tools
pip install -e .
```

### Requirements

- Python 3.7 or higher
- requests library (automatically installed)

## Future Enhancements

1. **Interactive Mode**: Interactive shell for continuous database administration
2. **Batch Operations**: Execute multiple commands from a script file
3. **Configuration File**: Support for configuration files to store connection details
4. **Enhanced Output**: Colorized output and progress indicators
5. **Autocompletion**: Shell autocompletion for commands and database/table names
6. **Backup/Restore**: Commands for database backup and restore operations
7. **Monitoring**: Real-time monitoring and alerting capabilities
8. **Security**: Authentication and authorization support

## Conclusion

The CLI tool provides a robust and easy-to-use interface for administering PhantomDB instances. The implementation follows Python best practices and provides comprehensive functionality for all PhantomDB operations through a command line interface.