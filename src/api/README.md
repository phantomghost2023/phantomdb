# PhantomDB REST API

RESTful API for PhantomDB - A distributed SQL database

## Overview

The PhantomDB REST API provides programmatic access to all database functionality through standard HTTP methods. The API follows REST conventions and returns JSON responses.

## Base URL

```
http://localhost:8080/api/v1
```

For production deployments:
```
https://api.phantomdb.com/v1
```

## Authentication

All API requests require authentication using JWT tokens. Include the token in the Authorization header:

```
Authorization: Bearer <your-jwt-token>
```

## API Endpoints

### Databases

#### List All Databases
```
GET /databases
```

Retrieve a list of all databases in the system.

#### Create Database
```
POST /databases
```

Create a new database with the specified name.

Request body:
```json
{
  "name": "mydatabase",
  "options": {
    "replicationFactor": 3,
    "shardCount": 4
  }
}
```

#### Get Database Information
```
GET /databases/{databaseName}
```

Retrieve information about a specific database.

#### Delete Database
```
DELETE /databases/{databaseName}
```

Delete a specific database and all its contents.

### Tables

#### List Tables in Database
```
GET /databases/{databaseName}/tables
```

Retrieve a list of all tables in a specific database.

#### Create Table
```
POST /databases/{databaseName}/tables
```

Create a new table in the specified database.

Request body:
```json
{
  "name": "users",
  "columns": [
    {
      "name": "id",
      "type": "INT",
      "primaryKey": true,
      "nullable": false
    },
    {
      "name": "name",
      "type": "VARCHAR(255)",
      "nullable": false
    },
    {
      "name": "email",
      "type": "VARCHAR(255)",
      "nullable": true
    }
  ],
  "options": {
    "autoIndex": true
  }
}
```

#### Get Table Information
```
GET /databases/{databaseName}/tables/{tableName}
```

Retrieve information about a specific table.

#### Delete Table
```
DELETE /databases/{databaseName}/tables/{tableName}
```

Delete a specific table and all its contents.

### Rows

#### Query Table Rows
```
GET /databases/{databaseName}/tables/{tableName}/rows
```

Query rows from a table with optional filtering and sorting.

Query parameters:
- `where`: SQL WHERE clause for filtering
- `orderBy`: SQL ORDER BY clause for sorting
- `limit`: Maximum number of rows to return (1-10000)
- `offset`: Number of rows to skip

Example:
```
GET /databases/testdb/tables/users/rows?where=name='John'&orderBy=id&limit=10
```

#### Insert Rows
```
POST /databases/{databaseName}/tables/{tableName}/rows
```

Insert one or more rows into a table.

Request body:
```json
{
  "rows": [
    {
      "id": 1,
      "name": "John Doe",
      "email": "john@example.com"
    },
    {
      "id": 2,
      "name": "Jane Smith",
      "email": "jane@example.com"
    }
  ]
}
```

### SQL Queries

#### Execute SQL Query
```
POST /databases/{databaseName}/query
```

Execute a SQL query against the specified database.

Request body:
```json
{
  "query": "SELECT * FROM users WHERE id = ?",
  "parameters": [1]
}
```

## Error Responses

All error responses follow the same format:

```json
{
  "code": "ERROR_CODE",
  "message": "Human-readable error message",
  "details": {
    "additional": "error details"
  }
}
```

Common HTTP status codes:
- `400`: Bad Request - Invalid request parameters
- `401`: Unauthorized - Missing or invalid authentication
- `404`: Not Found - Resource not found
- `409`: Conflict - Resource already exists
- `500`: Internal Server Error - Server-side error

## Rate Limiting

The API implements rate limiting to prevent abuse:
- 1000 requests per hour per IP address
- 100 requests per minute per IP address

Exceeding these limits will result in a `429 Too Many Requests` response.

## Examples

### Python Client Example

```python
import requests

# Set up the base URL and headers
base_url = "http://localhost:8080/api/v1"
headers = {
    "Authorization": "Bearer your-jwt-token",
    "Content-Type": "application/json"
}

# Create a database
response = requests.post(
    f"{base_url}/databases",
    headers=headers,
    json={"name": "testdb"}
)

if response.status_code == 201:
    print("Database created successfully")
else:
    print(f"Error: {response.status_code} - {response.json()}")

# Create a table
table_data = {
    "name": "users",
    "columns": [
        {"name": "id", "type": "INT", "primaryKey": True, "nullable": False},
        {"name": "name", "type": "VARCHAR(255)", "nullable": False},
        {"name": "email", "type": "VARCHAR(255)", "nullable": True}
    ]
}

response = requests.post(
    f"{base_url}/databases/testdb/tables",
    headers=headers,
    json=table_data
)

if response.status_code == 201:
    print("Table created successfully")
else:
    print(f"Error: {response.status_code} - {response.json()}")

# Insert data
rows_data = {
    "rows": [
        {"id": 1, "name": "John Doe", "email": "john@example.com"},
        {"id": 2, "name": "Jane Smith", "email": "jane@example.com"}
    ]
}

response = requests.post(
    f"{base_url}/databases/testdb/tables/users/rows",
    headers=headers,
    json=rows_data
)

if response.status_code == 201:
    print(f"Inserted {response.json()['inserted']} rows")
else:
    print(f"Error: {response.status_code} - {response.json()}")

# Query data
response = requests.get(
    f"{base_url}/databases/testdb/tables/users/rows?where=id=1",
    headers=headers
)

if response.status_code == 200:
    data = response.json()
    print(f"Found {data['count']} rows")
    for row in data['rows']:
        print(row)
else:
    print(f"Error: {response.status_code} - {response.json()}")
```

### JavaScript Client Example

```javascript
// Using fetch API
const baseUrl = 'http://localhost:8080/api/v1';
const headers = {
    'Authorization': 'Bearer your-jwt-token',
    'Content-Type': 'application/json'
};

// Create a database
async function createDatabase(name) {
    try {
        const response = await fetch(`${baseUrl}/databases`, {
            method: 'POST',
            headers: headers,
            body: JSON.stringify({ name: name })
        });
        
        if (response.ok) {
            const data = await response.json();
            console.log('Database created:', data);
            return data;
        } else {
            const error = await response.json();
            console.error('Error:', error);
            throw new Error(error.message);
        }
    } catch (error) {
        console.error('Network error:', error);
        throw error;
    }
}

// Query data
async function queryUsers() {
    try {
        const response = await fetch(
            `${baseUrl}/databases/testdb/tables/users/rows?where=id=1`,
            { headers: headers }
        );
        
        if (response.ok) {
            const data = await response.json();
            console.log('Users:', data.rows);
            return data.rows;
        } else {
            const error = await response.json();
            console.error('Error:', error);
            throw new Error(error.message);
        }
    } catch (error) {
        console.error('Network error:', error);
        throw error;
    }
}

// Usage
createDatabase('testdb')
    .then(() => queryUsers())
    .catch(error => console.error('Operation failed:', error));
```

## API Versioning

The API uses URL versioning. The current version is `v1`. Breaking changes will be introduced in new versions (e.g., `v2`).

## WebSockets

For real-time notifications and streaming queries, PhantomDB also supports WebSocket connections:

```
ws://localhost:8080/ws/v1
```

WebSocket connections require the same JWT authentication token to be provided during the handshake.

## SDKs

Official SDKs are available for:
- Python
- JavaScript/Node.js
- Java
- Go
- C#

## Support

For API support, please:
1. Check the documentation
2. Review the OpenAPI specification
3. Contact support@phantomdb.com
4. File issues on GitHub

## Changelog

### v1.0.0 (Current)
- Initial release
- Full CRUD operations for databases, tables, and rows
- SQL query execution
- Authentication and authorization
- Rate limiting