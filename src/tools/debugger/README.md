# PhantomDB Query Debugger and Profiler

A command line tool for debugging and profiling SQL queries in PhantomDB.

## Installation

```bash
pip install -e .
```

## Usage

### Basic Usage

```bash
# Execute a query with debugging information
phantomdb-debugger.py execute mydb "SELECT * FROM users" --debug

# Profile a query by running it multiple times
phantomdb-debugger.py profile mydb "SELECT * FROM users WHERE id = 1" --iterations 20

# Explain query execution plan
phantomdb-debugger.py explain mydb "SELECT * FROM users"

# Show query execution history
phantomdb-debugger.py history

# Clear query execution history
phantomdb-debugger.py clear-history
```

### Specifying Server URL

By default, the debugger connects to `http://localhost:8080`. You can specify a different server URL:

```bash
phantomdb-debugger.py --url http://example.com:8080 execute mydb "SELECT * FROM users"
```

## Commands

### Execute Query

Execute a query with optional debugging information:

```bash
phantomdb-debugger.py execute <database> "<query>" [--debug]
```

### Profile Query

Profile a query by executing it multiple times and collecting statistics:

```bash
phantomdb-debugger.py profile <database> "<query>" [--iterations N]
```

### Explain Query

Get the execution plan for a query (simulated in this implementation):

```bash
phantomdb-debugger.py explain <database> "<query>"
```

### Query History

Show the history of executed queries with timing information:

```bash
phantomdb-debugger.py history
```

Clear the query history:

```bash
phantomdb-debugger.py clear-history
```

## Features

- **Query Execution**: Execute SQL queries with detailed timing information
- **Query Profiling**: Run queries multiple times to collect performance statistics
- **Query Explanation**: Get execution plan information for queries
- **Query History**: Track executed queries with timing and result information
- **Debugging Output**: Detailed information about query execution including timing and result size

## Requirements

- Python 3.7 or higher
- requests library

## License

MIT License