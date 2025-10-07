# PhantomDB Sequelize Integration

Sequelize ORM integration for PhantomDB - A next-generation database system.

## Installation

```bash
npm install phantomdb-sequelize
```

Note: You also need to have the PhantomDB JavaScript client installed:

```bash
npm install phantomdb-js
```

## Usage

### Basic Setup

```javascript
const { Sequelize } = require('sequelize');
const { PhantomDBDialect } = require('phantomdb-sequelize');

// Create PhantomDB dialect instance
const phantomDialect = new PhantomDBDialect('http://localhost:8080');

// Initialize Sequelize with PhantomDB
const sequelize = new Sequelize('database_name', null, null, {
  dialect: phantomDialect,
  host: 'localhost',
  port: 8080
});
```

### Defining Models

```javascript
const { DataTypes } = require('sequelize');

// Define a User model
const User = sequelize.define('User', {
  // Model attributes are defined here
  firstName: {
    type: DataTypes.STRING,
    allowNull: false
  },
  lastName: {
    type: DataTypes.STRING
    // allowNull defaults to true
  },
  email: {
    type: DataTypes.STRING,
    allowNull: false,
    unique: true
  }
}, {
  // Other model options go here
  tableName: 'users'
});

// Sync the model with the database
await User.sync({ force: true });
```

### CRUD Operations

```javascript
// Create a new user
const user = await User.create({
  firstName: 'John',
  lastName: 'Doe',
  email: 'john.doe@example.com'
});
console.log('User created:', user.toJSON());

// Find all users
const users = await User.findAll();
console.log('All users:', JSON.stringify(users, null, 2));

// Find a user by ID
const userById = await User.findByPk(1);
console.log('User by ID:', userById.toJSON());

// Find users with specific attributes
const usersByEmail = await User.findAll({
  where: {
    email: 'john.doe@example.com'
  }
});
console.log('Users by email:', JSON.stringify(usersByEmail, null, 2));

// Update a user
await User.update({
  lastName: 'Smith'
}, {
  where: {
    id: 1
  }
});

// Delete a user
await User.destroy({
  where: {
    id: 1
  }
});
```

### Querying

```javascript
// Find users with complex queries
const users = await User.findAll({
  where: {
    firstName: 'John',
    email: {
      [Op.like]: '%example.com'
    }
  },
  order: [
    ['firstName', 'ASC'],
    ['lastName', 'DESC']
  ],
  limit: 10,
  offset: 0
});
```

## Features

### Supported Sequelize Features

1. **Model Definition**: Define models with attributes and options
2. **CRUD Operations**: Create, Read, Update, Delete operations
3. **Advanced Querying**: Complex queries with WHERE clauses, operators, ordering, limits
4. **Data Types**: Mapping of Sequelize data types to PhantomDB types
5. **Model Synchronization**: Sync models with database schema
6. **Transactions**: Basic transaction support
7. **Raw Queries**: Execute raw SQL queries

### PhantomDB Specific Features

1. **REST API Integration**: Seamless integration with PhantomDB REST API
2. **Health Checking**: Automatic connection health monitoring
3. **Error Handling**: Comprehensive error handling with meaningful messages
4. **Asynchronous Operations**: Fully asynchronous with Promise support

## Data Type Mapping

| Sequelize Type | PhantomDB Type |
|----------------|----------------|
| INTEGER        | integer        |
| BIGINT         | integer        |
| STRING         | string         |
| TEXT           | string         |
| BOOLEAN        | boolean        |
| DATE           | timestamp      |
| FLOAT          | float          |
| DOUBLE         | float          |

## Enhanced Features

The current implementation now includes enhanced features:

1. **Advanced Queries**: Support for complex WHERE clauses with operators (eq, ne, gt, gte, lt, lte, like, in)
2. **Ordering**: Support for ORDER BY clauses
3. **Pagination**: Support for LIMIT and OFFSET
4. **Transactions**: Basic transaction support (beginTransaction, commitTransaction, rollbackTransaction)
5. **Raw Queries**: Support for executing raw SQL queries
6. **Model Registration**: Support for registering models and associations

## Limitations

The current implementation still has the following limitations:

1. **Associations**: Model associations (hasOne, hasMany, etc.) are not yet fully implemented
2. **Migrations**: Sequelize migration support is not implemented
3. **Complex Joins**: Advanced JOIN operations are not yet supported
4. **Aggregations**: Aggregation functions are not yet implemented

## Example Application

```javascript
const { Sequelize, DataTypes } = require('sequelize');
const { PhantomDBDialect } = require('phantomdb-sequelize');

async function main() {
  try {
    // Create PhantomDB dialect instance
    const phantomDialect = new PhantomDBDialect('http://localhost:8080');
    
    // Initialize Sequelize
    const sequelize = new Sequelize('myapp', null, null, {
      dialect: phantomDialect
    });
    
    // Define models
    const User = sequelize.define('User', {
      firstName: {
        type: DataTypes.STRING,
        allowNull: false
      },
      lastName: {
        type: DataTypes.STRING
      },
      email: {
        type: DataTypes.STRING,
        allowNull: false,
        unique: true
      }
    });
    
    // Sync model
    await User.sync({ force: true });
    
    // Create a user
    const user = await User.create({
      firstName: 'John',
      lastName: 'Doe',
      email: 'john.doe@example.com'
    });
    
    console.log('Created user:', user.toJSON());
    
    // Find all users
    const users = await User.findAll();
    console.log('All users:', JSON.stringify(users, null, 2));
    
  } catch (error) {
    console.error('Error:', error.message);
  }
}

main();
```

## Testing

To run tests:

```bash
npm test
```

## Requirements

- Node.js >= 12.0.0
- PhantomDB server running on specified host/port
- phantomdb-js client library
- Sequelize >= 6.0.0

## License

MIT