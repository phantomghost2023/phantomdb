"""
Unit tests for the PhantomDB Python client.
"""

import unittest
from unittest.mock import patch, Mock
import json
from phantomdb import PhantomDB, PhantomDBError


class TestPhantomDB(unittest.TestCase):
    """Test cases for the PhantomDB client."""
    
    def setUp(self):
        """Set up test fixtures before each test method."""
        self.db = PhantomDB("http://localhost:8080")
    
    @patch('phantomdb.requests.Session.request')
    def test_health_check(self, mock_request):
        """Test health check functionality."""
        # Mock response
        mock_response = Mock()
        mock_response.json.return_value = {"status": "healthy"}
        mock_response.content = json.dumps({"status": "healthy"}).encode()
        mock_response.raise_for_status.return_value = None
        mock_request.return_value = mock_response
        
        # Call the method
        result = self.db.health_check()
        
        # Assertions
        self.assertEqual(result, {"status": "healthy"})
        mock_request.assert_called_once_with("GET", "http://localhost:8080/health")
    
    @patch('phantomdb.requests.Session.request')
    def test_list_databases(self, mock_request):
        """Test listing databases."""
        # Mock response
        mock_response = Mock()
        mock_response.json.return_value = {"databases": ["db1", "db2"]}
        mock_response.content = json.dumps({"databases": ["db1", "db2"]}).encode()
        mock_response.raise_for_status.return_value = None
        mock_request.return_value = mock_response
        
        # Call the method
        result = self.db.list_databases()
        
        # Assertions
        self.assertEqual(result, ["db1", "db2"])
        mock_request.assert_called_once_with("GET", "http://localhost:8080/databases")
    
    @patch('phantomdb.requests.Session.request')
    def test_create_database(self, mock_request):
        """Test creating a database."""
        # Mock response
        mock_response = Mock()
        mock_response.json.return_value = {"message": "Database created"}
        mock_response.content = json.dumps({"message": "Database created"}).encode()
        mock_response.raise_for_status.return_value = None
        mock_request.return_value = mock_response
        
        # Call the method
        result = self.db.create_database("testdb")
        
        # Assertions
        self.assertEqual(result, {"message": "Database created"})
        mock_request.assert_called_once_with(
            "POST", 
            "http://localhost:8080/databases",
            json={"name": "testdb"},
            headers={'Content-Type': 'application/json'}
        )
    
    @patch('phantomdb.requests.Session.request')
    def test_request_exception(self, mock_request):
        """Test handling of request exceptions."""
        # Mock exception
        mock_request.side_effect = Exception("Connection failed")
        
        # Assertions
        with self.assertRaises(PhantomDBError):
            self.db.health_check()


if __name__ == '__main__':
    unittest.main()