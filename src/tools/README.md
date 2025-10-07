# PhantomDB Command Line Tools

Command line tools for administering PhantomDB instances, including both a traditional CLI and an interactive REPL.

## Installation

```bash
pip install phantomdb-cli
```

Or install directly from source:

```bash
pip install -e .
```

## Usage

### Traditional CLI

```bash
# Check server health
phantomdb-cli health

# List databases
phantomdb-cli list-databases

# Create a database
phantomdb-cli create-database myapp

# Drop a database
phantomdb-cli drop-database myapp

# List tables in a database
phantomdb-cli list-tables myapp

# Create a table
phantomdb-cli create-table myapp users id:int name:string email:string

# Drop a table
phantomdb-cli drop-table myapp users

# Execute a custom SQL query
phantomdb-cli execute-query myapp "SELECT * FROM users"

# Begin a transaction
phantomdb-cli begin-transaction

# Commit a transaction
phantomdb-cli commit-transaction txn_12345

# Rollback a transaction
phantomdb-cli rollback-transaction txn_12345
```

### Interactive REPL

Launch the interactive REPL:

```bash
# Launch REPL with default server URL
phantomdb-repl

# Launch REPL with custom server URL
phantomdb-repl --url http://example.com:8080
```

Once in the REPL, you can use the following commands:

- `help` - Show available commands
- `use <database>` - Select a database to work with
- `show databases` - List all databases
- `show tables` - List all tables in current database
- `exit` or `quit` - Exit the REPL
- `history` - Show command history

You can also execute SQL queries directly:

```sql
SELECT * FROM users;
INSERT INTO users (id, name, email) VALUES (1, 'John Doe', 'john@example.com');
UPDATE users SET email = 'john.doe@example.com' WHERE id = 1;
DELETE FROM users WHERE id = 1;
```

### Specifying Server URL

By default, the CLI connects to `http://localhost:8080`. You can specify a different server URL:

```bash
phantomdb-cli --url http://example.com:8080 health
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

### Import/Export Operations

Import data from a file:

```bash
phantomdb-cli import <database_name> <table_name> <file_path> --format [csv|json]
```

Export data to a file:

```bash
phantomdb-cli export <database_name> <table_name> <file_path> --format [csv|json]
```

## Query Debugger and Profiler

PhantomDB also includes a query debugger and profiler tool for analyzing query performance:

```bash
# Execute a query with debugging information
phantomdb-debugger execute myapp "SELECT * FROM users" --debug

# Profile a query by running it multiple times
phantomdb-debugger profile myapp "SELECT * FROM users WHERE id = 1" --iterations 20

# Explain query execution plan
phantomdb-debugger explain myapp "SELECT * FROM users"

# Show query execution history
phantomdb-debugger history
```

See the [debugger documentation](debugger/README.md) for more details.

## Requirements

- Python 3.7 or higher
- requests library

## License

MIT License