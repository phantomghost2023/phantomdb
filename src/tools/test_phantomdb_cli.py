"""
Unit tests for the PhantomDB CLI tool.
"""

import unittest
from unittest.mock import patch, Mock
import json
import sys
from io import StringIO
from phantomdb_cli import PhantomDBCLI


class TestPhantomDBCLI(unittest.TestCase):
    """Test cases for the PhantomDB CLI."""
    
    def setUp(self):
        """Set up test fixtures before each test method."""
        self.cli = PhantomDBCLI("http://localhost:8080")
    
    @patch('phantomdb_cli.requests.Session.request')
    def test_health_check_success(self, mock_request):
        """Test health check functionality with successful response."""
        # Mock response
        mock_response = Mock()
        mock_response.json.return_value = {"status": "healthy"}
        mock_response.content = json.dumps({"status": "healthy"}).encode()
        mock_response.raise_for_status.return_value = None
        mock_request.return_value = mock_response
        
        # Capture stdout
        captured_output = StringIO()
        sys.stdout = captured_output
        
        # Call the method
        self.cli.health_check()
        
        # Reset stdout
        sys.stdout = sys.__stdout__
        
        # Assertions
        self.assertIn("Server health", captured_output.getvalue())
        mock_request.assert_called_once_with("GET", "http://localhost:8080/health")
    
    @patch('phantomdb_cli.requests.Session.request')
    def test_health_check_error(self, mock_request):
        """Test health check functionality with error response."""
        # Mock exception
        mock_request.side_effect = Exception("Connection failed")
        
        # Capture stdout
        captured_output = StringIO()
        sys.stdout = captured_output
        
        # Call the method
        self.cli.health_check()
        
        # Reset stdout
        sys.stdout = sys.__stdout__
        
        # Assertions
        self.assertIn("Health check failed", captured_output.getvalue())
    
    @patch('phantomdb_cli.requests.Session.request')
    def test_list_databases_success(self, mock_request):
        """Test listing databases with successful response."""
        # Mock response
        mock_response = Mock()
        mock_response.json.return_value = {"databases": ["db1", "db2"]}
        mock_response.content = json.dumps({"databases": ["db1", "db2"]}).encode()
        mock_response.raise_for_status.return_value = None
        mock_request.return_value = mock_response
        
        # Capture stdout
        captured_output = StringIO()
        sys.stdout = captured_output
        
        # Call the method
        self.cli.list_databases()
        
        # Reset stdout
        sys.stdout = sys.__stdout__
        
        # Assertions
        self.assertIn("db1", captured_output.getvalue())
        self.assertIn("db2", captured_output.getvalue())
        mock_request.assert_called_once_with("GET", "http://localhost:8080/databases")
    
    @patch('phantomdb_cli.requests.Session.request')
    def test_create_database_success(self, mock_request):
        """Test creating a database with successful response."""
        # Mock response
        mock_response = Mock()
        mock_response.json.return_value = {"message": "Database created"}
        mock_response.content = json.dumps({"message": "Database created"}).encode()
        mock_response.raise_for_status.return_value = None
        mock_request.return_value = mock_response
        
        # Capture stdout
        captured_output = StringIO()
        sys.stdout = captured_output
        
        # Call the method
        self.cli.create_database("testdb")
        
        # Reset stdout
        sys.stdout = sys.__stdout__
        
        # Assertions
        self.assertIn("Database 'testdb' created successfully", captured_output.getvalue())
        mock_request.assert_called_once_with(
            "POST", 
            "http://localhost:8080/databases",
            json={"name": "testdb"},
            headers={'Content-Type': 'application/json'}
        )


if __name__ == '__main__':
    unittest.main()