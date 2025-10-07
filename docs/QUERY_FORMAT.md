# PhantomDB Query Format Specification

PhantomDB supports a simple JSON-based query format that allows users to perform database operations without writing SQL.

## Query Structure

All queries follow this basic structure:

```json
{
  "operation": "select|insert|update|delete",
  "database": "database_name",
  "table": "table_name",
  "fields": ["field1", "field2", ...],
  "conditions": {
    "field": "value",
    "field2": "value2"
  },
  "data": {
    "field1": "value1",
    "field2": "value2"
  },
  "limit": 10,
  "offset": 0
}
```

## SELECT Queries

### Basic SELECT

```json
{
  "operation": "select",
  "database": "testdb",
  "table": "users"
}
```

### SELECT with Fields

```json
{
  "operation": "select",
  "database": "testdb",
  "table": "users",
  "fields": ["id", "name", "email"]
}
```

### SELECT with Conditions

```json
{
  "operation": "select",
  "database": "testdb",
  "table": "users",
  "conditions": {
    "id": "1",
    "name": "John Doe"
  }
}
```

### SELECT with Limit and Offset

```json
{
  "operation": "select",
  "database": "testdb",
  "table": "users",
  "limit": 10,
  "offset": 20
}
```

## INSERT Queries

### Basic INSERT

```json
{
  "operation": "insert",
  "database": "testdb",
  "table": "users",
  "data": {
    "id": "1",
    "name": "John Doe",
    "email": "john@example.com",
    "age": "30"
  }
}
```

## UPDATE Queries

### UPDATE with Conditions

```json
{
  "operation": "update",
  "database": "testdb",
  "table": "users",
  "data": {
    "age": "31",
    "email": "john.doe.updated@example.com"
  },
  "conditions": {
    "id": "1"
  }
}
```

## DELETE Queries

### DELETE with Conditions

```json
{
  "operation": "delete",
  "database": "testdb",
  "table": "users",
  "conditions": {
    "id": "1"
  }
}
```

### DELETE All Rows

```json
{
  "operation": "delete",
  "database": "testdb",
  "table": "users"
}
```

## Response Format

### Success Response

```json
{
  "success": true,
  "data": [...],
  "count": 5,
  "message": "Operation completed successfully"
}
```

### Error Response

```json
{
  "success": false,
  "error": "Error message",
  "code": "ERROR_CODE"
}
```

## Data Types

PhantomDB supports the following data types:

- `string` - Text data
- `integer` - Whole numbers
- `boolean` - true/false values
- `float` - Decimal numbers
- `timestamp` - Date and time values

## Operators

For conditions, the following operators are supported:

- `=` - Equal to
- `!=` - Not equal to
- `>` - Greater than
- `<` - Less than
- `>=` - Greater than or equal to
- `<=` - Less than or equal to
- `LIKE` - Pattern matching
- `IN` - Value in a list

Example with operators:

```json
{
  "operation": "select",
  "database": "testdb",
  "table": "users",
  "conditions": {
    "age": {">": "25"},
    "name": {"LIKE": "John%"},
    "id": {"IN": ["1", "2", "3"]}
  }
}
```

## Complex Queries

### JOIN Operations

```json
{
  "operation": "select",
  "database": "testdb",
  "tables": ["users", "orders"],
  "join": {
    "type": "inner",
    "on": {
      "users.id": "orders.user_id"
    }
  },
  "fields": ["users.name", "orders.total"]
}
```

### Aggregation

```json
{
  "operation": "select",
  "database": "testdb",
  "table": "orders",
  "aggregate": {
    "count": "*",
    "sum": "total",
    "avg": "total",
    "group_by": ["user_id"]
  }
}
```

This query format provides a simple, consistent way to interact with PhantomDB without requiring knowledge of SQL syntax.