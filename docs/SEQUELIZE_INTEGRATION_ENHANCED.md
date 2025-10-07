# Enhanced Sequelize Integration for PhantomDB

This document describes the enhanced features of the Sequelize integration for PhantomDB, including advanced querying, transactions, and model management.

## Overview

The enhanced Sequelize integration provides a more complete dialect implementation that supports advanced Sequelize features while maintaining compatibility with PhantomDB's REST API.

## Enhanced Features

### 1. Advanced Querying

The enhanced dialect now supports complex WHERE clauses with Sequelize operators:

```javascript
const { Op } = require('sequelize');

// Complex queries with operators
const users = await User.findAll({
  where: {
    age: {
      [Op.gt]: 18,
      [Op.lt]: 65
    },
    name: {
      [Op.like]: '%john%'
    },
    status: {
      [Op.in]: ['active', 'pending']
    }
  },
  order: [
    ['createdAt', 'DESC'],
    ['name', 'ASC']
  ],
  limit: 10,
  offset: 0
});
```

#### Supported Operators

| Operator | Description | Example |
|----------|-------------|---------|
| [Op.eq] | Equal | `{ age: { [Op.eq]: 25 } }` |
| [Op.ne] | Not equal | `{ status: { [Op.ne]: 'inactive' } }` |
| [Op.gt] | Greater than | `{ age: { [Op.gt]: 18 } }` |
| [Op.gte] | Greater than or equal | `{ age: { [Op.gte]: 18 } }` |
| [Op.lt] | Less than | `{ age: { [Op.lt]: 65 } }` |
| [Op.lte] | Less than or equal | `{ age: { [Op.lte]: 65 } }` |
| [Op.like] | LIKE operator | `{ name: { [Op.like]: '%john%' } }` |
| [Op.in] | IN operator | `{ status: { [Op.in]: ['active', 'pending'] } }` |

### 2. Model Management

The enhanced dialect includes model registration and association support:

```javascript
// Register models
const User = sequelize.define('User', {
  firstName: DataTypes.STRING,
  lastName: DataTypes.STRING
});

// Register the model with the dialect
sequelize.dialect.registerModel('User', User);

// Define associations (planned for future implementation)
// User.hasMany(Post);
// Post.belongsTo(User);
```

### 3. Transaction Support

Basic transaction support is now available:

```javascript
// Begin a transaction
const transaction = await sequelize.transaction();

try {
  // Perform operations within the transaction
  await User.create({ firstName: 'John', lastName: 'Doe' }, { transaction });
  await Profile.create({ bio: 'Software Developer' }, { transaction });
  
  // Commit the transaction
  await transaction.commit();
} catch (error) {
  // Rollback the transaction
  await transaction.rollback();
  throw error;
}
```

### 4. Raw Query Support

Execute raw SQL queries directly:

```javascript
// Execute a raw query
const result = await sequelize.query('SELECT * FROM users WHERE age > 18', {
  type: sequelize.QueryTypes.SELECT
});
```

## Implementation Details

### Core Components

#### 1. Enhanced PhantomDBDialect Class

The enhanced dialect includes additional methods and features:

```javascript
class PhantomDBDialect {
  constructor(connectionString) {
    this.connectionString = connectionString || 'http://localhost:8080';
    this.db = new PhantomDB(this.connectionString);
    this.models = new Map(); // Store model definitions
    this.associations = new Map(); // Store associations
  }
  
  // Model registration
  registerModel(modelName, modelDefinition) {
    this.models.set(modelName, modelDefinition);
  }
  
  // Association definition
  defineAssociation(sourceModel, targetModel, associationType, options = {}) {
    const associationKey = `${sourceModel}-${targetModel}`;
    this.associations.set(associationKey, {
      source: sourceModel,
      target: targetModel,
      type: associationType,
      options
    });
  }
  
  // Transaction support
  async beginTransaction() {
    return await this.db.beginTransaction();
  }
  
  async commitTransaction(transactionId) {
    return await this.db.commitTransaction(transactionId);
  }
  
  async rollbackTransaction(transactionId) {
    return await this.db.rollbackTransaction(transactionId);
  }
  
  // Raw query execution
  async query(databaseName, query) {
    return await this.db.executeQuery(databaseName, query);
  }
}
```

#### 2. Enhanced Query Building

Advanced query building with operator support:

```javascript
_buildWhereClause(where) {
  if (!where) return null;
  
  const conditions = [];
  for (const [key, value] of Object.entries(where)) {
    // Handle different operators
    if (typeof value === 'object' && value !== null) {
      for (const [operator, opValue] of Object.entries(value)) {
        switch (operator) {
          case Sequelize.Op.eq:
            if (typeof opValue === 'string') {
              conditions.push(`${key} = '${opValue}'`);
            } else {
              conditions.push(`${key} = ${opValue}`);
            }
            break;
          case Sequelize.Op.ne:
            if (typeof opValue === 'string') {
              conditions.push(`${key} != '${opValue}'`);
            } else {
              conditions.push(`${key} != ${opValue}`);
            }
            break;
          case Sequelize.Op.gt:
            conditions.push(`${key} > ${opValue}`);
            break;
          // ... additional operators
        }
      }
    } else {
      // Simple equality
      if (typeof value === 'string') {
        conditions.push(`${key} = '${value}'`);
      } else {
        conditions.push(`${key} = ${value}`);
      }
    }
  }
  
  return conditions.join(' AND ');
}
```

### File Structure

```
src/orm/sequelize/
├── package.json          # NPM package configuration
├── index.js              # Main dialect implementation
├── README.md             # Documentation
├── example.js            # Usage examples
└── test.js               # Tests
```

## Usage Examples

### Advanced Querying

```javascript
const { Sequelize, DataTypes, Op } = require('sequelize');
const { PhantomDBDialect } = require('phantomdb-sequelize');

const sequelize = new Sequelize('myapp', null, null, {
  dialect: new PhantomDBDialect('http://localhost:8080')
});

const User = sequelize.define('User', {
  firstName: DataTypes.STRING,
  lastName: DataTypes.STRING,
  email: DataTypes.STRING,
  age: DataTypes.INTEGER,
  status: DataTypes.STRING
});

// Complex query with multiple conditions
const activeAdults = await User.findAll({
  where: {
    age: {
      [Op.gt]: 18,
      [Op.lt]: 65
    },
    status: 'active',
    email: {
      [Op.like]: '%@example.com'
    }
  },
  order: [
    ['firstName', 'ASC'],
    ['lastName', 'DESC']
  ],
  limit: 20,
  offset: 0
});
```

### Transactions

```javascript
// Transaction example
async function createUserWithProfile(userData, profileData) {
  const transaction = await sequelize.transaction();
  
  try {
    // Create user
    const user = await User.create(userData, { transaction });
    
    // Create profile
    const profile = await Profile.create({
      ...profileData,
      userId: user.id
    }, { transaction });
    
    // Commit transaction
    await transaction.commit();
    
    return { user, profile };
  } catch (error) {
    // Rollback on error
    await transaction.rollback();
    throw error;
  }
}
```

### Raw Queries

```javascript
// Execute raw SQL
const result = await sequelize.query(
  'SELECT u.firstName, u.lastName, p.bio FROM users u JOIN profiles p ON u.id = p.userId WHERE u.age > :age',
  {
    replacements: { age: 25 },
    type: Sequelize.QueryTypes.SELECT
  }
);
```

## Testing

The enhanced implementation includes comprehensive tests:

1. **Unit Tests**
   - Model registration
   - Association definition
   - Query building
   - Transaction methods

2. **Integration Tests**
   - Complex query execution
   - Transaction workflows
   - Raw query execution

## Dependencies

The enhanced implementation requires:

- **sequelize**: >= 6.0.0
- **phantomdb-js**: PhantomDB JavaScript client
- **Node.js**: >= 12.0.0

## Future Enhancements

Planned enhancements include:

1. **Full Association Support**
   - hasOne, hasMany, belongsTo, belongsToMany
   - Eager loading
   - Association queries

2. **Migration Support**
   - Sequelize migration integration
   - Schema versioning
   - Rollback operations

3. **Advanced Features**
   - Aggregation functions
   - Subquery support
   - Complex JOIN operations

4. **Performance Optimizations**
   - Connection pooling
   - Query caching
   - Batch operations

## Summary

The enhanced Sequelize integration for PhantomDB provides a more complete ORM experience with support for advanced querying, transactions, and model management. This implementation significantly improves the developer experience while maintaining compatibility with PhantomDB's unique features and performance characteristics.

The enhanced dialect now supports:
- Complex WHERE clauses with Sequelize operators
- ORDER BY and LIMIT/OFFSET clauses
- Model registration and basic association support
- Transaction management
- Raw query execution
- Comprehensive error handling

This foundation enables developers to leverage familiar Sequelize patterns while benefiting from PhantomDB's advanced database capabilities.