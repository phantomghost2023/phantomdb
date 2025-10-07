/**
 * Example usage of PhantomDB Sequelize integration
 */

const { Sequelize, DataTypes } = require('sequelize');

// Mock PhantomDBDialect for demonstration
// In a real implementation, this would be imported from the actual module
class PhantomDBDialect {
  constructor(connectionString) {
    this.connectionString = connectionString || 'http://localhost:8080';
    this.models = new Map();
    this.associations = new Map();
  }

  async init(sequelize) {
    console.log('Initializing PhantomDB dialect with connection:', this.connectionString);
    // In a real implementation, this would check the connection
  }

  registerModel(modelName, modelDefinition) {
    this.models.set(modelName, modelDefinition);
    console.log(`Registered model: ${modelName}`);
  }

  defineAssociation(sourceModel, targetModel, associationType, options = {}) {
    const associationKey = `${sourceModel}-${targetModel}`;
    this.associations.set(associationKey, {
      source: sourceModel,
      target: targetModel,
      type: associationType,
      options
    });
    console.log(`Defined association: ${sourceModel} -> ${targetModel} (${associationType})`);
  }

  async createTable(databaseName, tableName, attributes) {
    console.log(`Creating table ${tableName} in database ${databaseName} with attributes:`, attributes);
    // In a real implementation, this would call the PhantomDB API
    return { message: `Table ${tableName} created successfully` };
  }

  async dropTable(databaseName, tableName) {
    console.log(`Dropping table ${tableName} from database ${databaseName}`);
    // In a real implementation, this would call the PhantomDB API
    return { message: `Table ${tableName} dropped successfully` };
  }

  async insert(databaseName, tableName, values) {
    console.log(`Inserting data into ${tableName}:`, values);
    // In a real implementation, this would call the PhantomDB API
    return { message: 'Data inserted successfully', id: Math.floor(Math.random() * 1000) };
  }

  async select(databaseName, tableName, options = {}) {
    console.log(`Selecting data from ${tableName} with options:`, options);
    // In a real implementation, this would call the PhantomDB API
    return [
      { id: 1, firstName: 'John', lastName: 'Doe', email: 'john.doe@example.com' },
      { id: 2, firstName: 'Jane', lastName: 'Smith', email: 'jane.smith@example.com' }
    ];
  }

  async update(databaseName, tableName, values, options = {}) {
    console.log(`Updating data in ${tableName} with values:`, values, 'and options:', options);
    // In a real implementation, this would call the PhantomDB API
    return { message: 'Data updated successfully', count: 1 };
  }

  async delete(databaseName, tableName, options = {}) {
    console.log(`Deleting data from ${tableName} with options:`, options);
    // In a real implementation, this would call the PhantomDB API
    return { message: 'Data deleted successfully', count: 1 };
  }

  async query(databaseName, query) {
    console.log(`Executing query on database ${databaseName}: ${query}`);
    // In a real implementation, this would call the PhantomDB API
    return { message: 'Query executed successfully' };
  }

  async beginTransaction() {
    console.log('Beginning transaction');
    // In a real implementation, this would call the PhantomDB API
    return 'txn_' + Math.random().toString(36).substr(2, 9);
  }

  async commitTransaction(transactionId) {
    console.log(`Committing transaction ${transactionId}`);
    // In a real implementation, this would call the PhantomDB API
    return { message: 'Transaction committed successfully' };
  }

  async rollbackTransaction(transactionId) {
    console.log(`Rolling back transaction ${transactionId}`);
    // In a real implementation, this would call the PhantomDB API
    return { message: 'Transaction rolled back successfully' };
  }

  _mapDataType(sequelizeType) {
    if (sequelizeType === DataTypes.INTEGER || sequelizeType === DataTypes.BIGINT) {
      return 'integer';
    } else if (sequelizeType === DataTypes.STRING || sequelizeType === DataTypes.TEXT) {
      return 'string';
    } else if (sequelizeType === DataTypes.BOOLEAN) {
      return 'boolean';
    } else if (sequelizeType === DataTypes.DATE) {
      return 'timestamp';
    } else if (sequelizeType === DataTypes.FLOAT || sequelizeType === DataTypes.DOUBLE) {
      return 'float';
    } else {
      return 'string';
    }
  }

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
            case Sequelize.Op.gte:
              conditions.push(`${key} >= ${opValue}`);
              break;
            case Sequelize.Op.lt:
              conditions.push(`${key} < ${opValue}`);
              break;
            case Sequelize.Op.lte:
              conditions.push(`${key} <= ${opValue}`);
              break;
            case Sequelize.Op.like:
              conditions.push(`${key} LIKE '${opValue}'`);
              break;
            case Sequelize.Op.in:
              const inValues = opValue.map(v => typeof v === 'string' ? `'${v}'` : v).join(', ');
              conditions.push(`${key} IN (${inValues})`);
              break;
            default:
              if (typeof opValue === 'string') {
                conditions.push(`${key} = '${opValue}'`);
              } else {
                conditions.push(`${key} = ${opValue}`);
              }
          }
        }
      } else {
        if (typeof value === 'string') {
          conditions.push(`${key} = '${value}'`);
        } else {
          conditions.push(`${key} = ${value}`);
        }
      }
    }
    
    return conditions.join(' AND ');
  }
}

async function main() {
  try {
    console.log('PhantomDB Sequelize Integration Example');
    console.log('=====================================');
    
    // Create PhantomDB dialect instance
    const phantomDialect = new PhantomDBDialect('http://localhost:8080');
    
    // Initialize Sequelize (mock implementation)
    console.log('\n1. Initializing Sequelize with PhantomDB dialect...');
    await phantomDialect.init();
    
    // Define a User model
    console.log('\n2. Defining User model...');
    
    // Mock model definition
    const User = {
      name: 'User',
      attributes: {
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
        },
        age: {
          type: DataTypes.INTEGER
        }
      }
    };
    
    console.log('User model defined with attributes:', User.attributes);
    
    // Register the model
    console.log('\n3. Registering User model...');
    phantomDialect.registerModel('User', User);
    
    // Sync the model (create table)
    console.log('\n4. Syncing User model (creating table)...');
    const syncResult = await phantomDialect.createTable('myapp', 'users', User.attributes);
    console.log('Sync result:', syncResult);
    
    // Create a new user
    console.log('\n5. Creating a new user...');
    const newUser = {
      firstName: 'John',
      lastName: 'Doe',
      email: 'john.doe@example.com',
      age: 30
    };
    
    const createResult = await phantomDialect.insert('myapp', 'users', newUser);
    console.log('Create result:', createResult);
    
    // Find all users
    console.log('\n6. Finding all users...');
    const users = await phantomDialect.select('myapp', 'users');
    console.log('All users:', users);
    
    // Find users with complex queries
    console.log('\n7. Finding users with complex queries...');
    const complexUsers = await phantomDialect.select('myapp', 'users', {
      where: {
        age: {
          [Sequelize.Op.gt]: 25
        },
        firstName: {
          [Sequelize.Op.like]: 'J%'
        }
      },
      order: [
        ['firstName', 'ASC'],
        ['lastName', 'DESC']
      ],
      limit: 10,
      offset: 0
    });
    console.log('Complex query users:', complexUsers);
    
    // Find a user by ID
    console.log('\n8. Finding user by ID...');
    const userById = await phantomDialect.select('myapp', 'users', { where: { id: 1 } });
    console.log('User by ID:', userById);
    
    // Update a user
    console.log('\n9. Updating a user...');
    const updateResult = await phantomDialect.update(
      'myapp', 
      'users', 
      { lastName: 'Smith' }, 
      { where: { id: 1 } }
    );
    console.log('Update result:', updateResult);
    
    // Execute a raw query
    console.log('\n10. Executing a raw query...');
    const queryResult = await phantomDialect.query('myapp', 'SELECT * FROM users WHERE age > 25');
    console.log('Query result:', queryResult);
    
    // Demonstrate transactions
    console.log('\n11. Demonstrating transactions...');
    const transactionId = await phantomDialect.beginTransaction();
    console.log('Transaction ID:', transactionId);
    
    try {
      // Perform operations within transaction
      const newUser2 = {
        firstName: 'Jane',
        lastName: 'Smith',
        email: 'jane.smith@example.com',
        age: 28
      };
      
      await phantomDialect.insert('myapp', 'users', newUser2);
      console.log('User inserted within transaction');
      
      // Commit the transaction
      const commitResult = await phantomDialect.commitTransaction(transactionId);
      console.log('Commit result:', commitResult);
    } catch (error) {
      // Rollback on error
      console.log('Error occurred, rolling back transaction:', error.message);
      await phantomDialect.rollbackTransaction(transactionId);
    }
    
    // Delete a user
    console.log('\n12. Deleting a user...');
    const deleteResult = await phantomDialect.delete('myapp', 'users', { where: { id: 1 } });
    console.log('Delete result:', deleteResult);
    
    // Drop the table
    console.log('\n13. Dropping the users table...');
    const dropResult = await phantomDialect.dropTable('myapp', 'users');
    console.log('Drop result:', dropResult);
    
    console.log('\nExample completed successfully!');
    
  } catch (error) {
    console.error('Error in example:', error.message);
  }
}

// Run the example
if (require.main === module) {
  main();
}

module.exports = { main };