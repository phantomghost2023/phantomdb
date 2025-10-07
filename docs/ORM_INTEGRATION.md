# ORM/ODM Integration for PhantomDB

This document describes the implementation of ORM/ODM integrations for PhantomDB, starting with Sequelize for Node.js.

## Overview

ORM/ODM integrations provide developers with familiar interfaces for working with PhantomDB, leveraging existing knowledge and patterns from popular frameworks while maintaining the unique capabilities of PhantomDB.

## Sequelize Integration

### Implementation Details

The Sequelize integration provides a custom dialect that bridges Sequelize's ORM capabilities with PhantomDB's REST API.

#### Core Components

1. **PhantomDBDialect Class**
   - Main dialect implementation
   - Connection management
   - Data type mapping
   - Query translation

2. **Data Type Mapping**
   - Sequelize to PhantomDB type conversion
   - Automatic schema generation
   - Validation support

3. **Query Translation**
   - WHERE clause conversion
   - ORDER BY translation
   - LIMIT/OFFSET handling

### Key Features

#### Seamless Integration

```javascript
const { Sequelize, DataTypes } = require('sequelize');
const { PhantomDBDialect } = require('phantomdb-sequelize');

const sequelize = new Sequelize('database_name', null, null, {
  dialect: new PhantomDBDialect('http://localhost:8080')
});
```

#### Model Definition

```javascript
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
```

#### CRUD Operations

```javascript
// Create
const user = await User.create({
  firstName: 'John',
  lastName: 'Doe',
  email: 'john.doe@example.com'
});

// Read
const users = await User.findAll();
const userById = await User.findByPk(1);

// Update
await User.update({
  lastName: 'Smith'
}, {
  where: {
    id: 1
  }
});

// Delete
await User.destroy({
  where: {
    id: 1
  }
});
```

### Implementation Approach

#### 1. Dialect Architecture

The PhantomDB dialect follows Sequelize's dialect pattern:

```javascript
class PhantomDBDialect {
  constructor(connectionString) {
    this.connectionString = connectionString;
    this.db = new PhantomDB(connectionString);
  }
  
  async init(sequelize) {
    // Initialize connection
  }
  
  async createTable(databaseName, tableName, attributes) {
    // Convert attributes and create table
  }
}
```

#### 2. Data Type Mapping

Seamless conversion between Sequelize and PhantomDB types:

| Sequelize Type | PhantomDB Type | Notes |
|----------------|----------------|-------|
| INTEGER        | integer        | 32-bit integer |
| BIGINT         | integer        | 64-bit integer |
| STRING         | string         | Variable length string |
| TEXT           | string         | Long text |
| BOOLEAN        | boolean        | True/False |
| DATE           | timestamp      | ISO 8601 format |
| FLOAT          | float          | 32-bit floating point |
| DOUBLE         | float          | 64-bit floating point |

#### 3. Query Translation

Conversion of Sequelize queries to PhantomDB API calls:

```javascript
// Sequelize query
User.findAll({
  where: {
    firstName: 'John',
    age: {
      [Op.gt]: 18
    }
  },
  order: [['createdAt', 'DESC']],
  limit: 10
});

// Translated to PhantomDB API calls
// GET /databases/myapp/tables/users?condition=firstName='John' AND age>18
// With additional processing for ordering and limits
```

### File Structure

```
src/orm/sequelize/
├── package.json          # NPM package configuration
├── index.js              # Main dialect implementation
├── example.js            # Usage examples
├── test.js               # Simple tests
└── README.md             # Documentation
```

### Supported Features

#### Phase 1 (Current Implementation)

1. **Basic Model Definition**
   - Attribute definition with data types
   - Model synchronization
   - Table creation/dropping

2. **CRUD Operations**
   - Create (INSERT)
   - Read (SELECT)
   - Update (UPDATE)
   - Delete (DELETE)

3. **Simple Queries**
   - Basic WHERE clauses
   - Equality conditions
   - String and numeric comparisons

#### Phase 2 (Future Enhancements)

1. **Advanced Queries**
   - Complex WHERE clauses
   - JOIN operations
   - Subqueries
   - Aggregation functions

2. **Model Associations**
   - hasOne, hasMany, belongsTo, belongsToMany
   - Eager loading
   - Association queries

3. **Transactions**
   - Transaction support
   - Rollback capabilities
   - Isolation levels

4. **Migrations**
   - Schema migration support
   - Version control
   - Rollback operations

### Usage Examples

#### Basic Example

```javascript
const { Sequelize, DataTypes } = require('sequelize');
const { PhantomDBDialect } = require('phantomdb-sequelize');

async function main() {
  // Initialize Sequelize with PhantomDB
  const sequelize = new Sequelize('myapp', null, null, {
    dialect: new PhantomDBDialect('http://localhost:8080')
  });
  
  // Define a model
  const User = sequelize.define('User', {
    firstName: DataTypes.STRING,
    lastName: DataTypes.STRING,
    email: {
      type: DataTypes.STRING,
      unique: true
    }
  });
  
  // Sync model with database
  await User.sync({ force: true });
  
  // Create a user
  const user = await User.create({
    firstName: 'John',
    lastName: 'Doe',
    email: 'john.doe@example.com'
  });
  
  console.log('Created user:', user.toJSON());
}
```

#### Query Example

```javascript
// Find users with specific criteria
const users = await User.findAll({
  where: {
    firstName: 'John',
    email: {
      [Sequelize.Op.like]: '%example.com'
    }
  },
  order: [
    ['firstName', 'ASC'],
    ['lastName', 'DESC']
  ],
  limit: 10
});

console.log('Found users:', users.map(u => u.toJSON()));
```

### Testing

The Sequelize integration includes:

1. **Unit Tests**
   - Data type mapping verification
   - Query translation tests
   - Error handling tests

2. **Integration Tests**
   - Model definition tests
   - CRUD operation tests
   - Query execution tests

### Dependencies

The Sequelize integration requires:

- **sequelize**: Sequelize ORM library
- **phantomdb-js**: PhantomDB JavaScript client
- **Node.js**: >= 12.0.0

### Future ORM/ODM Integrations

Planned integrations include:

1. **Python Django ORM**
2. **Python SQLAlchemy**
3. **Go GORM**
4. **Rust Diesel**
5. **MongoDB ODM-like interface**

### Benefits

1. **Developer Productivity**
   - Familiar APIs for existing developers
   - Reduced learning curve
   - Leverage existing documentation and tutorials

2. **Code Reusability**
   - Existing models can be adapted
   - Shared patterns and practices
   - Community support

3. **Ecosystem Integration**
   - Works with existing tools
   - Compatible with popular frameworks
   - Standardized interfaces

### Challenges

1. **Feature Parity**
   - Not all Sequelize features may be supported initially
   - Some PhantomDB-specific features may not map directly
   - Performance considerations

2. **Query Translation**
   - Complex queries may require special handling
   - Optimization opportunities
   - Error mapping

3. **Data Type Compatibility**
   - Some type conversions may be lossy
   - Validation requirements
   - Default value handling

## Next Steps

1. **Complete Sequelize Implementation**
   - Add support for associations
   - Implement advanced query features
   - Add transaction support

2. **Additional ORM Integrations**
   - Django ORM for Python
   - SQLAlchemy for Python
   - GORM for Go

3. **Documentation and Examples**
   - Comprehensive guides
   - Migration guides from other databases
   - Best practices documentation

4. **Testing and Validation**
   - Extensive test coverage
   - Performance benchmarking
   - Compatibility testing

The ORM/ODM integration significantly enhances PhantomDB's developer experience by providing familiar interfaces while maintaining the unique capabilities and performance characteristics of PhantomDB.