# PhantomDB Query Debugger and Profiler

## Overview

The PhantomDB Query Debugger and Profiler is a development tool that helps developers analyze and optimize SQL queries in PhantomDB. It provides detailed timing information, query profiling capabilities, execution plan analysis, and query history tracking.

## Features

1. **Query Execution with Debugging**: Execute queries with detailed timing and result information
2. **Query Profiling**: Run queries multiple times to collect performance statistics
3. **Query Explanation**: Analyze query execution plans
4. **Query History**: Track executed queries with timing and result information
5. **Performance Analysis**: Identify slow queries and optimization opportunities

## Installation

The debugger is included with the PhantomDB tools package. To install:

```bash
cd src/tools
pip install -e .
```

## Usage

### Command Line Interface

The debugger can be used as a command line tool:

```bash
# Execute a query with debugging information
phantomdb-debugger execute mydb "SELECT * FROM users" --debug

# Profile a query by running it multiple times
phantomdb-debugger profile mydb "SELECT * FROM users WHERE id = 1" --iterations 20

# Explain query execution plan
phantomdb-debugger explain mydb "SELECT * FROM users"

# Show query execution history
phantomdb-debugger history
```

### Programmatic Usage

The debugger can also be used programmatically in Python applications:

```python
from debugger.phantomdb_debugger import QueryDebugger

# Create a debugger instance
debugger = QueryDebugger("http://localhost:8080")

# Execute a query with debugging information
result = debugger.execute_query("myapp", "SELECT * FROM users", debug=True)

# Profile a query
profile_result = debugger.profile_query("myapp", "SELECT * FROM users WHERE id = 1", iterations=10)

# Explain a query
explanation = debugger.explain_query("myapp", "SELECT u.name, o.total FROM users u JOIN orders o ON u.id = o.user_id")
```

## Command Reference

### execute

Execute a query with optional debugging information:

```
phantomdb-debugger execute <database> "<query>" [--debug]
```

Options:
- `--debug`: Enable detailed debugging output including timing and result size information

### profile

Profile a query by executing it multiple times and collecting statistics:

```
phantomdb-debugger profile <database> "<query>" [--iterations N]
```

Options:
- `--iterations N`: Number of iterations to run (default: 10)

### explain

Get the execution plan for a query:

```
phantomdb-debugger explain <database> "<query>"
```

### history

Show the history of executed queries with timing information:

```
phantomdb-debugger history
```

### clear-history

Clear the query execution history:

```
phantomdb-debugger clear-history
```

## Output Formats

### Debug Output

When using the `--debug` flag with the execute command, the output includes:

- Query execution time in milliseconds
- Start and end timestamps
- Result size in bytes
- Query result data

### Profile Output

When profiling a query, the output includes:

- Average execution time across all iterations
- Minimum execution time
- Maximum execution time
- Individual execution times for each iteration

### Explanation Output

When explaining a query, the output includes:

- Query execution plan information
- Estimated cost
- Table and column information
- Filter conditions

## Query History

The debugger maintains a history of executed queries with:

- Query text
- Database name
- Execution time
- Timestamps
- Result size

This history can be viewed with the `history` command and cleared with the `clear-history` command.

## Integration with Development Workflows

The debugger can be integrated into development workflows to:

1. Identify performance bottlenecks in queries
2. Compare query performance before and after optimization
3. Validate query correctness with detailed result information
4. Track query performance over time

## Best Practices

1. **Use in Development**: Use the debugger during development to identify and fix performance issues early
2. **Profile Regularly**: Regularly profile queries to ensure performance doesn't degrade over time
3. **Compare Results**: Compare profiling results before and after making changes to queries or schema
4. **Monitor History**: Use query history to track performance trends and identify problematic queries

## Limitations

1. The current implementation simulates query explanation rather than providing actual execution plans
2. Performance profiling is limited to client-side timing and does not include server-side metrics
3. The tool requires a running PhantomDB server to connect to

## Future Enhancements

Planned enhancements for the debugger include:

1. Real execution plan analysis from the database engine
2. Server-side performance metrics integration
3. Advanced query analysis and optimization suggestions
4. Integration with observability tools like Prometheus and Grafana
5. Visual query plan display
6. Query comparison tools