# PhantomDB CLI

An interactive command-line interface for PhantomDB with autocomplete and query history.

## Features

- Interactive REPL (Read-Eval-Print Loop)
- SQL command autocomplete
- Command history with navigation
- Syntax highlighting (planned)
- Query execution and result display

## Usage

To start the CLI, run:

```bash
./phantomdb-cli
```

### Commands

- `help` - Show available commands
- `history` - Show command history
- `clear` - Clear command history
- `exit` or `quit` - Exit the CLI
- Any valid SQL statement

### Navigation

- Use arrow keys to navigate command history
- Tab key for autocomplete suggestions (planned)

## Building

To build the CLI tool:

```bash
cd tools/cli
mkdir build
cd build
cmake ..
make
```

## Examples

```sql
-- Create a database
CREATE DATABASE testdb;

-- Use the database
USE testdb;

-- Create a table
CREATE TABLE users (
    id INT PRIMARY KEY,
    name VARCHAR(50),
    email VARCHAR(100)
);

-- Insert data
INSERT INTO users (id, name, email) VALUES (1, 'John Doe', 'john@example.com');

-- Query data
SELECT * FROM users WHERE id = 1;

-- Update data
UPDATE users SET email = 'john.doe@example.com' WHERE id = 1;

-- Delete data
DELETE FROM users WHERE id = 1;
```