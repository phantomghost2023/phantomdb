/**
 * Example usage of the PhantomDB JavaScript client.
 */

const { PhantomDB, PhantomDBError } = require('./phantomdb.js');

async function main() {
  /** Demonstrate PhantomDB JavaScript client usage. */
  
  // Connect to PhantomDB server
  const db = new PhantomDB('http://localhost:8080');
  
  try {
    // Check server health
    console.log('Checking server health...');
    const health = await db.healthCheck();
    console.log('Server health:', health);
    
    // Create a database
    console.log('\nCreating database \'example_db\'...');
    let result = await db.createDatabase('example_db');
    console.log('Create database result:', result);
    
    // List databases
    console.log('\nListing databases...');
    const databases = await db.listDatabases();
    console.log('Databases:', databases);
    
    // Create a table
    console.log('\nCreating table \'users\'...');
    const columns = [
      { name: 'id', type: 'int' },
      { name: 'name', type: 'string' },
      { name: 'email', type: 'string' }
    ];
    result = await db.createTable('example_db', 'users', columns);
    console.log('Create table result:', result);
    
    // List tables
    console.log('\nListing tables...');
    const tables = await db.listTables('example_db');
    console.log('Tables:', tables);
    
    // Insert data
    console.log('\nInserting data...');
    let userData = {
      id: '1',
      name: 'John Doe',
      email: 'john@example.com'
    };
    result = await db.insert('example_db', 'users', userData);
    console.log('Insert result:', result);
    
    // Insert another record
    userData = {
      id: '2',
      name: 'Jane Smith',
      email: 'jane@example.com'
    };
    result = await db.insert('example_db', 'users', userData);
    console.log('Insert result:', result);
    
    // Query data
    console.log('\nQuerying data...');
    let users = await db.select('example_db', 'users');
    console.log('Users:', users);
    
    // Update data
    console.log('\nUpdating data...');
    result = await db.update('example_db', 'users', { name: 'Jane Doe' }, 'id = 2');
    console.log('Update result:', result);
    
    // Query updated data
    console.log('\nQuerying updated data...');
    users = await db.select('example_db', 'users');
    console.log('Users:', users);
    
    // Delete data
    console.log('\nDeleting data...');
    result = await db.delete('example_db', 'users', 'id = 1');
    console.log('Delete result:', result);
    
    // Query remaining data
    console.log('\nQuerying remaining data...');
    users = await db.select('example_db', 'users');
    console.log('Users:', users);
    
    // Demonstrate transactions
    console.log('\nDemonstrating transactions...');
    const txnId = await db.beginTransaction();
    console.log('Transaction ID:', txnId);
    
    try {
      // Insert data within transaction
      const userData3 = {
        id: '3',
        name: 'Bob Johnson',
        email: 'bob@example.com'
      };
      result = await db.insert('example_db', 'users', userData3);
      console.log('Insert in transaction result:', result);
      
      // Commit the transaction
      result = await db.commitTransaction(txnId);
      console.log('Commit result:', result);
    } catch (error) {
      // Rollback on error
      console.log('Error occurred, rolling back:', error.message);
      await db.rollbackTransaction(txnId);
    }
    
    // Final query
    console.log('\nFinal data query...');
    users = await db.select('example_db', 'users');
    console.log('Users:', users);
    
    // Drop table
    console.log('\nDropping table \'users\'...');
    result = await db.dropTable('example_db', 'users');
    console.log('Drop table result:', result);
    
    // Drop database
    console.log('\nDropping database \'example_db\'...');
    result = await db.dropDatabase('example_db');
    console.log('Drop database result:', result);
    
  } catch (error) {
    if (error instanceof PhantomDBError) {
      console.log('PhantomDB error:', error.message);
    } else {
      console.log('Unexpected error:', error.message);
    }
  }
}

// Run the example if this file is executed directly
if (require.main === module) {
  main();
}

module.exports = { main };