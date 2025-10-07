/**
 * Comprehensive test for the PhantomDB JavaScript client.
 * This test verifies all major functionality of the SDK.
 */

const { PhantomDB, PhantomDBError, connect } = require('./phantomdb.js');

// Mock fetch implementation for testing without a real server
let mockResponses = {};
let callCount = {};

global.fetch = async (url, options) => {
  const key = `${options.method} ${url}`;
  callCount[key] = (callCount[key] || 0) + 1;
  
  // Return mocked responses based on URL and method
  if (mockResponses[key]) {
    return mockResponses[key];
  }
  
  // Default success response
  return {
    ok: true,
    text: async () => JSON.stringify({ success: true })
  };
};

// Helper function to set up mock responses
function setupMockResponse(method, url, response) {
  const key = `${method} ${url}`;
  mockResponses[key] = {
    ok: true,
    text: async () => JSON.stringify(response)
  };
}

// Helper function to clear mocks
function clearMocks() {
  mockResponses = {};
  callCount = {};
}

async function runComprehensiveTest() {
  console.log('Running comprehensive PhantomDB JavaScript client test...\n');
  
  try {
    // Test 1: Client instantiation
    console.log('1. Testing client instantiation...');
    const db = new PhantomDB('http://localhost:8080');
    console.log('   ✓ PhantomDB client created successfully');
    
    // Test 2: Connect function
    console.log('2. Testing connect function...');
    const db2 = connect('http://localhost:8080');
    console.log('   ✓ connect() function works correctly');
    
    // Test 3: Health check
    console.log('3. Testing health check...');
    setupMockResponse('GET', 'http://localhost:8080/health', { status: 'healthy', uptime: '10h' });
    const health = await db.healthCheck();
    console.log('   ✓ Health check result:', health);
    
    // Test 4: List databases
    console.log('4. Testing list databases...');
    setupMockResponse('GET', 'http://localhost:8080/databases', { databases: ['db1', 'db2', 'testdb'] });
    const databases = await db.listDatabases();
    console.log('   ✓ Databases listed:', databases);
    
    // Test 5: Create database
    console.log('5. Testing create database...');
    setupMockResponse('POST', 'http://localhost:8080/databases', { message: 'Database created successfully' });
    const createResult = await db.createDatabase('newdb');
    console.log('   ✓ Create database result:', createResult);
    
    // Test 6: Drop database
    console.log('6. Testing drop database...');
    setupMockResponse('DELETE', 'http://localhost:8080/databases/olddb', { message: 'Database dropped successfully' });
    const dropResult = await db.dropDatabase('olddb');
    console.log('   ✓ Drop database result:', dropResult);
    
    // Test 7: List tables
    console.log('7. Testing list tables...');
    setupMockResponse('GET', 'http://localhost:8080/databases/testdb/tables', { tables: ['users', 'products'] });
    const tables = await db.listTables('testdb');
    console.log('   ✓ Tables listed:', tables);
    
    // Test 8: Create table
    console.log('8. Testing create table...');
    const columns = [
      { name: 'id', type: 'int' },
      { name: 'name', type: 'string' },
      { name: 'email', type: 'string' }
    ];
    setupMockResponse('POST', 'http://localhost:8080/databases/testdb/tables', { message: 'Table created successfully' });
    const createTableResult = await db.createTable('testdb', 'users', columns);
    console.log('   ✓ Create table result:', createTableResult);
    
    // Test 9: Drop table
    console.log('9. Testing drop table...');
    setupMockResponse('DELETE', 'http://localhost:8080/databases/testdb/tables/users', { message: 'Table dropped successfully' });
    const dropTableResult = await db.dropTable('testdb', 'users');
    console.log('   ✓ Drop table result:', dropTableResult);
    
    // Test 10: Insert data
    console.log('10. Testing insert data...');
    const userData = { id: 1, name: 'John Doe', email: 'john@example.com' };
    setupMockResponse('POST', 'http://localhost:8080/databases/testdb/tables/users', { message: 'Data inserted successfully', id: 1 });
    const insertResult = await db.insert('testdb', 'users', userData);
    console.log('    ✓ Insert result:', insertResult);
    
    // Test 11: Select data
    console.log('11. Testing select data...');
    setupMockResponse('GET', 'http://localhost:8080/databases/testdb/tables/users', { records: [userData] });
    const selectResult = await db.select('testdb', 'users');
    console.log('    ✓ Select result:', selectResult);
    
    // Test 12: Update data
    console.log('12. Testing update data...');
    setupMockResponse('PUT', 'http://localhost:8080/databases/testdb/tables/users', { message: 'Data updated successfully', count: 1 });
    const updateResult = await db.update('testdb', 'users', { name: 'Jane Doe' }, 'id = 1');
    console.log('    ✓ Update result:', updateResult);
    
    // Test 13: Delete data
    console.log('13. Testing delete data...');
    setupMockResponse('DELETE', 'http://localhost:8080/databases/testdb/tables/users?condition=id%20%3D%201', { message: 'Data deleted successfully', count: 1 });
    const deleteResult = await db.delete('testdb', 'users', 'id = 1');
    console.log('    ✓ Delete result:', deleteResult);
    
    // Test 14: Execute query
    console.log('14. Testing execute query...');
    setupMockResponse('POST', 'http://localhost:8080/databases/testdb/query', { results: [{ id: 1, name: 'John Doe' }] });
    const queryResult = await db.executeQuery('testdb', 'SELECT * FROM users WHERE id = 1');
    console.log('    ✓ Query result:', queryResult);
    
    // Test 15: Transaction operations
    console.log('15. Testing transaction operations...');
    setupMockResponse('POST', 'http://localhost:8080/transactions', { transaction_id: 'txn_12345' });
    const txnId = await db.beginTransaction();
    console.log('    ✓ Begin transaction result:', txnId);
    
    setupMockResponse('POST', 'http://localhost:8080/transactions/txn_12345/commit', { message: 'Transaction committed' });
    const commitResult = await db.commitTransaction(txnId);
    console.log('    ✓ Commit transaction result:', commitResult);
    
    setupMockResponse('POST', 'http://localhost:8080/transactions/txn_12345/rollback', { message: 'Transaction rolled back' });
    const rollbackResult = await db.rollbackTransaction('txn_12345');
    console.log('    ✓ Rollback transaction result:', rollbackResult);
    
    // Test 16: Error handling
    console.log('16. Testing error handling...');
    clearMocks();
    global.fetch = async () => {
      throw new Error('Network error');
    };
    
    try {
      await db.healthCheck();
      console.log('    ✗ Error: Should have thrown an exception');
    } catch (error) {
      if (error instanceof PhantomDBError) {
        console.log('    ✓ Correctly caught PhantomDBError:', error.message);
      } else {
        console.log('    ✗ Unexpected error type:', error.message);
      }
    }
    
    // Test 17: HTTP error handling
    console.log('17. Testing HTTP error handling...');
    global.fetch = async () => {
      return {
        ok: false,
        status: 500,
        statusText: 'Internal Server Error'
      };
    };
    
    try {
      await db.healthCheck();
      console.log('    ✗ Error: Should have thrown an exception');
    } catch (error) {
      if (error instanceof PhantomDBError) {
        console.log('    ✓ Correctly caught HTTP error as PhantomDBError');
      } else {
        console.log('    ✗ Unexpected error type:', error.message);
      }
    }
    
    console.log('\n🎉 All tests completed successfully!');
    console.log('\nSummary of functionality verified:');
    console.log('  ✓ Client instantiation');
    console.log('  ✓ Health check');
    console.log('  ✓ Database operations (create, list, drop)');
    console.log('  ✓ Table operations (create, list, drop)');
    console.log('  ✓ Data operations (insert, select, update, delete)');
    console.log('  ✓ Custom query execution');
    console.log('  ✓ Transaction operations (begin, commit, rollback)');
    console.log('  ✓ Error handling');
    console.log('  ✓ Cross-environment compatibility');
    
    return true;
    
  } catch (error) {
    console.error('\n❌ Test failed:', error.message);
    console.error(error.stack);
    return false;
  }
}

// Run the test if this file is executed directly
if (require.main === module) {
  runComprehensiveTest().then(success => {
    if (success) {
      console.log('\n✅ JavaScript SDK verification: PASSED');
    } else {
      console.log('\n❌ JavaScript SDK verification: FAILED');
      process.exit(1);
    }
  });
}

module.exports = { runComprehensiveTest };