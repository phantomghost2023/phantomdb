/**
 * Simple test for PhantomDB Sequelize integration
 */

const { PhantomDBDialect } = require('./index.js');

// Mock PhantomDB client for testing
class MockPhantomDB {
  async healthCheck() {
    return { status: 'healthy' };
  }
  
  async createDatabase(name) {
    return { message: `Database ${name} created` };
  }
  
  async dropDatabase(name) {
    return { message: `Database ${name} dropped` };
  }
  
  async createTable(database, name, columns) {
    return { message: `Table ${name} created in ${database}` };
  }
  
  async dropTable(database, name) {
    return { message: `Table ${name} dropped from ${database}` };
  }
  
  async insert(database, table, data) {
    return { message: `Data inserted into ${table}`, id: 1 };
  }
  
  async select(database, table, condition) {
    return [{ id: 1, name: 'test' }];
  }
  
  async update(database, table, data, condition) {
    return { message: `Data updated in ${table}`, count: 1 };
  }
  
  async delete(database, table, condition) {
    return { message: `Data deleted from ${table}`, count: 1 };
  }
}

// Replace the actual PhantomDB client with mock for testing
// This is a simplified test - in a real implementation, we would use proper mocking

async function runTest() {
  console.log('Running PhantomDB Sequelize integration test...');
  
  try {
    // Test dialect creation
    console.log('\n1. Testing dialect creation...');
    const dialect = new PhantomDBDialect('http://localhost:8080');
    console.log('✓ Dialect created successfully');
    
    // Test model registration
    console.log('\n2. Testing model registration...');
    const { DataTypes } = require('sequelize');
    
    const User = {
      name: 'User',
      attributes: {
        id: { type: DataTypes.INTEGER },
        name: { type: DataTypes.STRING },
        active: { type: DataTypes.BOOLEAN }
      }
    };
    
    dialect.registerModel('User', User);
    console.log('✓ Model registered successfully');
    
    // Test association definition
    console.log('\n3. Testing association definition...');
    dialect.defineAssociation('User', 'Profile', 'hasOne');
    console.log('✓ Association defined successfully');
    
    // Test data type mapping
    console.log('\n4. Testing data type mapping...');
    const integerType = dialect._mapDataType(DataTypes.INTEGER);
    console.log('✓ INTEGER mapped to:', integerType);
    
    const stringType = dialect._mapDataType(DataTypes.STRING);
    console.log('✓ STRING mapped to:', stringType);
    
    const booleanType = dialect._mapDataType(DataTypes.BOOLEAN);
    console.log('✓ BOOLEAN mapped to:', booleanType);
    
    // Test simple WHERE clause building
    console.log('\n5. Testing simple WHERE clause building...');
    const simpleWhereClause = dialect._buildWhereClause({ id: 1, name: 'test' });
    console.log('✓ Simple WHERE clause built:', simpleWhereClause);
    
    // Test complex WHERE clause building
    console.log('\n6. Testing complex WHERE clause building...');
    const complexWhereClause = dialect._buildWhereClause({
      age: {
        [Sequelize.Op.gt]: 18
      },
      name: {
        [Sequelize.Op.like]: '%john%'
      }
    });
    console.log('✓ Complex WHERE clause built:', complexWhereClause);
    
    // Test ORDER BY clause building
    console.log('\n7. Testing ORDER BY clause building...');
    const orderByClause = dialect._buildOrderByClause([
      ['name', 'ASC'],
      ['age', 'DESC']
    ]);
    console.log('✓ ORDER BY clause built:', orderByClause);
    
    // Test LIMIT/OFFSET clause building
    console.log('\n8. Testing LIMIT/OFFSET clause building...');
    const limitOffsetClause = dialect._buildLimitOffsetClause({
      limit: 10,
      offset: 5
    });
    console.log('✓ LIMIT/OFFSET clause built:', limitOffsetClause);
    
    // Test transaction methods
    console.log('\n9. Testing transaction methods...');
    const transactionId = await dialect.beginTransaction();
    console.log('✓ Transaction begun with ID:', transactionId);
    
    const commitResult = await dialect.commitTransaction(transactionId);
    console.log('✓ Transaction committed:', commitResult.message);
    
    console.log('\nAll tests passed! ✓');
    
  } catch (error) {
    console.error('Test failed:', error.message);
    process.exit(1);
  }
}

// Run the test
if (require.main === module) {
  runTest();
}

module.exports = { runTest };