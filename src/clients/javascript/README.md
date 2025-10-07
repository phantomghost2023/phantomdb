# PhantomDB JavaScript Client

A JavaScript client library for interacting with PhantomDB through its REST API.

## Installation

```bash
npm install phantomdb-js
```

Or include directly in your HTML:

```html
<script src="phantomdb.js"></script>
```

## Usage

### Node.js

```javascript
const { PhantomDB, connect } = require('phantomdb-js');

// Connect to PhantomDB server
const db = new PhantomDB('http://localhost:8080');

// Or use the connect function
// const db = connect('http://localhost:8080');

// Check server health
async function example() {
  try {
    const health = await db.healthCheck();
    console.log('Server health:', health);
    
    // Create a database
    const result = await db.createDatabase('example_db');
    console.log('Create database result:', result);
    
    // List databases
    const databases = await db.listDatabases();
    console.log('Databases:', databases);
    
    // Create a table
    const columns = [
      { name: 'id', type: 'int' },
      { name: 'name', type: 'string' },
      { name: 'email', type: 'string' }
    ];
    const tableResult = await db.createTable('example_db', 'users', columns);
    console.log('Create table result:', tableResult);
    
    // Insert data
    const userData = {
      id: '1',
      name: 'John Doe',
      email: 'john@example.com'
    };
    const insertResult = await db.insert('example_db', 'users', userData);
    console.log('Insert result:', insertResult);
    
    // Query data
    const users = await db.select('example_db', 'users');
    console.log('Users:', users);
  } catch (error) {
    console.error('Error:', error.message);
  }
}

example();
```

### Browser

```html
<!DOCTYPE html>
<html>
<head>
    <title>PhantomDB JavaScript Client Example</title>
    <script src="phantomdb.js"></script>
</head>
<body>
    <script>
        // Connect to PhantomDB server
        const db = new PhantomDB.PhantomDB('http://localhost:8080');
        
        // Check server health
        db.healthCheck()
            .then(health => console.log('Server health:', health))
            .catch(error => console.error('Error:', error.message));
    </script>
</body>
</html>
```

## API Reference

### Constructor

```javascript
const db = new PhantomDB(baseUrl);
```

- `baseUrl` (string): The base URL of the PhantomDB REST API server. Defaults to `http://localhost:8080`.

### Methods

#### Health Check
```javascript
const health = await db.healthCheck();
```

#### Database Operations
- `listDatabases()`: List all databases
- `createDatabase(name)`: Create a new database
- `dropDatabase(name)`: Drop a database

#### Table Operations
- `listTables(database)`: List all tables in a database
- `createTable(database, name, columns)`: Create a new table
- `dropTable(database, name)`: Drop a table

#### Data Operations
- `insert(database, table, data)`: Insert data into a table
- `select(database, table, [condition])`: Select data from a table
- `update(database, table, data, [condition])`: Update data in a table
- `delete(database, table, [condition])`: Delete data from a table

#### Query Operations
- `executeQuery(database, query)`: Execute a custom SQL query

#### Transaction Operations
- `beginTransaction()`: Begin a new transaction
- `commitTransaction(transactionId)`: Commit a transaction
- `rollbackTransaction(transactionId)`: Rollback a transaction

## Error Handling

All methods can throw a `PhantomDBError` if the request fails:

```javascript
try {
  const result = await db.createDatabase('test');
} catch (error) {
  if (error instanceof PhantomDBError) {
    console.error('PhantomDB error:', error.message);
  } else {
    console.error('Unexpected error:', error.message);
  }
}
```

## Requirements

- Node.js >= 12.0.0 (for Node.js usage)
- Modern browser with Fetch API support (for browser usage)

## License

MIT