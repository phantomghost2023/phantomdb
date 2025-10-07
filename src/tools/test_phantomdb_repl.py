#!/usr/bin/env python3
"""
Unit tests for PhantomDB REPL
"""

import unittest
import sys
import os
from io import StringIO
from unittest.mock import patch, MagicMock
import requests

# Add the src/tools directory to the path so we can import the module
sys.path.insert(0, os.path.join(os.path.dirname(__file__)))

import phantomdb_repl


class TestPhantomDBREPL(unittest.TestCase):
    """Test cases for PhantomDB REPL functionality."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.repl = phantomdb_repl.PhantomDBREPL("http://localhost:8080")
    
    def test_init(self):
        """Test REPL initialization."""
        self.assertEqual(self.repl.base_url, "http://localhost:8080")
        self.assertIsNotNone(self.repl.session)
        self.assertIsNone(self.repl.current_database)
        self.assertIsInstance(self.repl.commands, dict)
        self.assertIsInstance(self.repl.sql_keywords, list)
    
    def test_completer_commands(self):
        """Test command autocompletion."""
        # Test completing commands
        result = self.repl.completer("sh", 0)
        self.assertEqual(result, "show databases")
        
        result = self.repl.completer("sh", 1)
        self.assertEqual(result, "show tables")
        
        # Test when no matches
        result = self.repl.completer("xyz", 0)
        self.assertIsNone(result)
    
    def test_completer_sql_keywords(self):
        """Test SQL keyword autocompletion."""
        # Test completing SQL keywords
        result = self.repl.completer("SEL", 0)
        self.assertEqual(result, "SELECT")
        
        result = self.repl.completer("FRO", 0)
        self.assertEqual(result, "FROM")
    
    @patch('phantomdb_repl.requests.Session')
    def test_make_request_success(self, mock_session):
        """Test successful HTTP request."""
        # Mock the response
        mock_response = MagicMock()
        mock_response.content = b'{"result": "success"}'
        mock_response.json.return_value = {"result": "success"}
        mock_response.raise_for_status.return_value = None
        
        # Mock the session request
        mock_session_instance = mock_session.return_value
        mock_session_instance.request.return_value = mock_response
        
        # Test the request
        result = self.repl._make_request("GET", "/test")
        self.assertEqual(result, {"result": "success"})
    
    @patch('phantomdb_repl.requests.Session')
    def test_make_request_error(self, mock_session):
        """Test HTTP request with error."""
        # Mock the response to raise an exception
        mock_session_instance = mock_session.return_value
        mock_session_instance.request.side_effect = requests.exceptions.RequestException("Connection error")
        
        # Test the request
        result = self.repl._make_request("GET", "/test")
        self.assertIn("error", result)
        self.assertEqual(result["error"], "Connection error")
    
    @patch('phantomdb_repl.PhantomDBREPL._make_request')
    def test_get_databases(self, mock_make_request):
        """Test getting databases."""
        # Mock the response
        mock_make_request.return_value = {"databases": ["db1", "db2", "db3"]}
        
        # Test getting databases
        result = self.repl.get_databases()
        self.assertEqual(result, ["db1", "db2", "db3"])
    
    @patch('phantomdb_repl.PhantomDBREPL._make_request')
    def test_get_tables(self, mock_make_request):
        """Test getting tables."""
        # Mock the response
        mock_make_request.return_value = {"tables": ["users", "orders", "products"]}
        
        # Test getting tables
        result = self.repl.get_tables("testdb")
        self.assertEqual(result, ["users", "orders", "products"])
    
    def test_show_help(self):
        """Test showing help."""
        # Capture stdout
        captured_output = StringIO()
        sys.stdout = captured_output
        
        # Test showing help
        self.repl.show_help()
        
        # Restore stdout
        sys.stdout = sys.__stdout__
        
        # Check that help was printed
        output = captured_output.getvalue()
        self.assertIn("PhantomDB Interactive REPL", output)
        self.assertIn("Available commands:", output)
    
    @patch('phantomdb_repl.PhantomDBREPL.get_databases')
    def test_select_database_success(self, mock_get_databases):
        """Test selecting a database successfully."""
        # Mock the response
        mock_get_databases.return_value = ["db1", "db2", "db3"]
        
        # Capture stdout
        captured_output = StringIO()
        sys.stdout = captured_output
        
        # Test selecting database
        self.repl.select_database("db2")
        
        # Restore stdout
        sys.stdout = sys.__stdout__
        
        # Check that database was selected
        self.assertEqual(self.repl.current_database, "db2")
        output = captured_output.getvalue()
        self.assertIn("Using database: db2", output)
    
    @patch('phantomdb_repl.PhantomDBREPL.get_databases')
    def test_select_database_not_found(self, mock_get_databases):
        """Test selecting a database that doesn't exist."""
        # Mock the response
        mock_get_databases.return_value = ["db1", "db2", "db3"]
        
        # Capture stdout
        captured_output = StringIO()
        sys.stdout = captured_output
        
        # Test selecting database
        self.repl.select_database("db4")
        
        # Restore stdout
        sys.stdout = sys.__stdout__
        
        # Check that error was printed
        output = captured_output.getvalue()
        self.assertIn("Database 'db4' not found.", output)


if __name__ == '__main__':
    unittest.main()