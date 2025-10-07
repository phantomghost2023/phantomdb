/**
 * Simple test for the PhantomDB JavaScript client that doesn't require a server.
 */

const { PhantomDB, PhantomDBError } = require('./phantomdb.js');

// Mock fetch for testing
global.fetch = async (url, options) => {
  // Simulate a successful health check response
  if (url === 'http://localhost:8080/health' && options.method === 'GET') {
    return {
      ok: true,
      text: async () => JSON.stringify({ status: 'healthy' })
    };
  }
  
  // Simulate a successful list databases response
  if (url === 'http://localhost:8080/databases' && options.method === 'GET') {
    return {
      ok: true,
      text: async () => JSON.stringify({ databases: ['db1', 'db2'] })
    };
  }
  
  // Simulate a successful create database response
  if (url === 'http://localhost:8080/databases' && options.method === 'POST') {
    return {
      ok: true,
      text: async () => JSON.stringify({ message: 'Database created' })
    };
  }
  
  // For other requests, simulate a generic success
  return {
    ok: true,
    text: async () => JSON.stringify({ success: true })
  };
};

async function runTest() {
  console.log('Running PhantomDB JavaScript client test...');
  
  try {
    // Create client
    const db = new PhantomDB('http://localhost:8080');
    
    // Test health check
    console.log('\n1. Testing health check...');
    const health = await db.healthCheck();
    console.log('Health check result:', health);
    
    // Test list databases
    console.log('\n2. Testing list databases...');
    const databases = await db.listDatabases();
    console.log('Databases:', databases);
    
    // Test create database
    console.log('\n3. Testing create database...');
    const result = await db.createDatabase('testdb');
    console.log('Create database result:', result);
    
    // Test error handling
    console.log('\n4. Testing error handling...');
    global.fetch = async () => {
      throw new Error('Network error');
    };
    
    try {
      await db.healthCheck();
      console.log('Error: Should have thrown an exception');
    } catch (error) {
      if (error instanceof PhantomDBError) {
        console.log('Correctly caught PhantomDBError:', error.message);
      } else {
        console.log('Unexpected error type:', error.message);
      }
    }
    
    console.log('\nAll tests completed successfully!');
    
  } catch (error) {
    console.error('Test failed:', error.message);
  }
}

runTest();