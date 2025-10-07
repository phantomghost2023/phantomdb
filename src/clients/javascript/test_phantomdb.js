/**
 * Unit tests for the PhantomDB JavaScript client.
 */

const { PhantomDB, PhantomDBError } = require('./phantomdb.js');

// Mock fetch for testing
global.fetch = jest.fn();

describe('PhantomDB', () => {
  let db;
  
  beforeEach(() => {
    db = new PhantomDB('http://localhost:8080');
    fetch.mockClear();
  });
  
  describe('healthCheck', () => {
    it('should check server health', async () => {
      // Mock response
      const mockResponse = {
        ok: true,
        text: async () => JSON.stringify({ status: 'healthy' })
      };
      
      fetch.mockResolvedValueOnce(mockResponse);
      
      // Call the method
      const result = await db.healthCheck();
      
      // Assertions
      expect(result).toEqual({ status: 'healthy' });
      expect(fetch).toHaveBeenCalledWith(
        'http://localhost:8080/health',
        {
          method: 'GET',
          headers: {
            'Content-Type': 'application/json'
          }
        }
      );
    });
  });
  
  describe('listDatabases', () => {
    it('should list databases', async () => {
      // Mock response
      const mockResponse = {
        ok: true,
        text: async () => JSON.stringify({ databases: ['db1', 'db2'] })
      };
      
      fetch.mockResolvedValueOnce(mockResponse);
      
      // Call the method
      const result = await db.listDatabases();
      
      // Assertions
      expect(result).toEqual(['db1', 'db2']);
      expect(fetch).toHaveBeenCalledWith(
        'http://localhost:8080/databases',
        {
          method: 'GET',
          headers: {
            'Content-Type': 'application/json'
          }
        }
      );
    });
  });
  
  describe('createDatabase', () => {
    it('should create a database', async () => {
      // Mock response
      const mockResponse = {
        ok: true,
        text: async () => JSON.stringify({ message: 'Database created' })
      };
      
      fetch.mockResolvedValueOnce(mockResponse);
      
      // Call the method
      const result = await db.createDatabase('testdb');
      
      // Assertions
      expect(result).toEqual({ message: 'Database created' });
      expect(fetch).toHaveBeenCalledWith(
        'http://localhost:8080/databases',
        {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify({ name: 'testdb' })
        }
      );
    });
  });
  
  describe('error handling', () => {
    it('should handle request exceptions', async () => {
      // Mock exception
      fetch.mockRejectedValueOnce(new Error('Connection failed'));
      
      // Assertions
      await expect(db.healthCheck()).rejects.toThrow(PhantomDBError);
    });
    
    it('should handle HTTP errors', async () => {
      // Mock HTTP error
      const mockResponse = {
        ok: false,
        status: 500,
        statusText: 'Internal Server Error'
      };
      
      fetch.mockResolvedValueOnce(mockResponse);
      
      // Assertions
      await expect(db.healthCheck()).rejects.toThrow(PhantomDBError);
    });
  });
});

// If running directly (not in Jest), run a simple test
if (typeof jest === 'undefined') {
  console.log('Running simple test...');
  
  // Mock fetch for simple test
  global.fetch = async (url, options) => {
    if (url === 'http://localhost:8080/health') {
      return {
        ok: true,
        text: async () => JSON.stringify({ status: 'healthy' })
      };
    }
    throw new Error('Unexpected URL');
  };
  
  async function runSimpleTest() {
    const db = new PhantomDB('http://localhost:8080');
    try {
      const health = await db.healthCheck();
      console.log('Simple test passed:', health);
    } catch (error) {
      console.error('Simple test failed:', error.message);
    }
  }
  
  runSimpleTest();
}